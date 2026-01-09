#include "AraDocumentController.h"

#include <algorithm>

void LocalAudioRecorder::prepare(double sampleRateIn,
                                 int numChannels,
                                 int maxSamplesPerBlock,
                                 double durationSeconds)
{
    sampleRate = sampleRateIn;
    const auto totalSamples = static_cast<int>(sampleRate * durationSeconds);
    recordedBuffer.setSize(std::max(1, numChannels), std::max(maxSamplesPerBlock, totalSamples));
    recordedBuffer.clear();
    writePosition = 0;
}

void LocalAudioRecorder::reset()
{
    recordedBuffer.clear();
    writePosition = 0;
}

void LocalAudioRecorder::pushBlock(const juce::AudioBuffer<float>& buffer)
{
    if (recordedBuffer.getNumSamples() == 0)
        return;

    const auto numSamples = buffer.getNumSamples();
    const auto totalSamples = recordedBuffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto writeIndex = static_cast<int>((writePosition + sample) % totalSamples);
        for (int channel = 0; channel < recordedBuffer.getNumChannels(); ++channel)
        {
            const auto* source = buffer.getReadPointer(std::min(channel, buffer.getNumChannels() - 1));
            auto* destination = recordedBuffer.getWritePointer(channel);
            destination[writeIndex] = source[sample];
        }
    }

    writePosition = (writePosition + numSamples) % totalSamples;
}

#if HIFITUNE_ENABLE_ARA
const juce::Identifier AraDocumentController::followHostCursorId{"followHostCursor"};
const juce::Identifier AraDocumentController::scrollToClipId{"scrollToClip"};
const juce::Identifier AraDocumentController::playbackTimeSecondsId{"playbackTimeSeconds"};
const juce::Identifier AraDocumentController::transportIsPlayingId{"transportIsPlaying"};
const juce::Identifier AraDocumentController::transportIsRecordingId{"transportIsRecording"};
const juce::Identifier AraDocumentController::transportIsLoopingId{"transportIsLooping"};

class AraDocumentController::AraPlaybackRenderer final : public juce::ARAPlaybackRenderer
{
public:
    explicit AraPlaybackRenderer(AraDocumentController& controller)
        : juce::ARAPlaybackRenderer(controller.getDocumentController()),
          owner(controller)
    {
    }

    bool processBlock(juce::AudioBuffer<float>& buffer,
                      juce::AudioProcessor::Realtime realtime,
                      const juce::AudioPlayHead::PositionInfo& positionInfo) noexcept override
    {
        owner.updateTransportFromHost(positionInfo);
        return juce::ARAPlaybackRenderer::processBlock(buffer, realtime, positionInfo);
    }

private:
    AraDocumentController& owner;
};

AraDocumentController::AraDocumentController(const ARA::PlugIn::PlugInEntry* entry,
                                             const ARA::ARADocumentControllerHostInstance* instance)
    : juce::ARADocumentControllerSpecialisation(entry, instance)
{
    ensureSettingsDefaults();
}

bool AraDocumentController::isFollowHostCursorEnabled() const
{
    return static_cast<bool>(projectState.getSettingsTree().getProperty(followHostCursorId, true));
}

bool AraDocumentController::isScrollToClipEnabled() const
{
    return static_cast<bool>(projectState.getSettingsTree().getProperty(scrollToClipId, true));
}

void AraDocumentController::setFollowHostCursorEnabled(bool enabled)
{
    projectState.getSettingsTree().setProperty(followHostCursorId, enabled, nullptr);
}

void AraDocumentController::setScrollToClipEnabled(bool enabled)
{
    projectState.getSettingsTree().setProperty(scrollToClipId, enabled, nullptr);
}

void AraDocumentController::updateTransportFromHost(const juce::AudioPlayHead::PositionInfo& positionInfo)
{
    lastPosition = positionInfo;

    auto sessionSettings = projectState.getSessionSettingsTree();
    sessionSettings.setProperty(playbackTimeSecondsId, positionInfo.getTimeInSeconds().orFallback(0.0), nullptr);
    sessionSettings.setProperty(transportIsPlayingId, positionInfo.getIsPlaying(), nullptr);
    sessionSettings.setProperty(transportIsRecordingId, positionInfo.getIsRecording(), nullptr);
    sessionSettings.setProperty(transportIsLoopingId, positionInfo.getIsLooping(), nullptr);
}

juce::String AraDocumentController::createStableId(const void* pointer)
{
    return juce::String::toHexString(reinterpret_cast<uint64_t>(pointer));
}

juce::ARAPlaybackRenderer* AraDocumentController::doCreatePlaybackRenderer() noexcept
{
    return new AraPlaybackRenderer(*this);
}

bool AraDocumentController::doRestoreObjectsFromStream(juce::ARAInputStream&, const juce::ARARestoreObjectsFilter*) noexcept
{
    ensureSettingsDefaults();
    return true;
}

bool AraDocumentController::doStoreObjectsToStream(juce::ARAOutputStream&, const juce::ARAStoreObjectsFilter*) noexcept
{
    return true;
}

void AraDocumentController::willBeginEditing(juce::ARADocument*)
{
    activeTransaction.emplace(projectState, "ARA Edit");
}

void AraDocumentController::didEndEditing(juce::ARADocument*)
{
    activeTransaction.reset();
}

void AraDocumentController::didUpdateRegionSequenceProperties(juce::ARARegionSequence* regionSequence)
{
    if (regionSequence == nullptr)
        return;

    const auto* properties = regionSequence->getProperties();
    if (properties == nullptr)
        return;

    const auto trackKey = createStableId(regionSequence);
    const auto name = juce::String(properties->name);
    regionMapper.syncTrack(trackKey, name, currentUndoManager());
}

void AraDocumentController::didAddPlaybackRegionToRegionSequence(juce::ARARegionSequence*,
                                                                 juce::ARAPlaybackRegion* playbackRegion)
{
    updatePlaybackRegion(playbackRegion);
}

void AraDocumentController::willRemovePlaybackRegionFromRegionSequence(juce::ARARegionSequence*,
                                                                       juce::ARAPlaybackRegion* playbackRegion)
{
    if (playbackRegion == nullptr)
        return;

    const auto regionKey = createStableId(playbackRegion);
    regionMapper.removeRegion(regionKey, currentUndoManager());
}

void AraDocumentController::didUpdatePlaybackRegionProperties(juce::ARAPlaybackRegion* playbackRegion)
{
    updatePlaybackRegion(playbackRegion);
}

void AraDocumentController::ensureSettingsDefaults()
{
    auto settingsTree = projectState.getSettingsTree();
    if (!settingsTree.hasProperty(followHostCursorId))
        settingsTree.setProperty(followHostCursorId, true, nullptr);
    if (!settingsTree.hasProperty(scrollToClipId))
        settingsTree.setProperty(scrollToClipId, true, nullptr);
}

void AraDocumentController::updatePlaybackRegion(juce::ARAPlaybackRegion* playbackRegion)
{
    if (playbackRegion == nullptr)
        return;

    const auto* properties = playbackRegion->getProperties();
    if (properties == nullptr)
        return;

    AraRegionMapper::RegionInfo info;
    info.regionKey = createStableId(playbackRegion);
    info.startBeat = static_cast<double>(properties->startInPlaybackTime);
    info.lengthBeats = static_cast<double>(properties->durationInPlaybackTime);

    regionMapper.mapRegionToClip(info, currentUndoManager());
    regionMapper.rebuildSegmentsForClip(info, currentUndoManager());
}

juce::UndoManager* AraDocumentController::currentUndoManager()
{
    return activeTransaction ? &projectState.getUndoManager() : nullptr;
}

const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory()
{
    return juce::ARADocumentControllerSpecialisation::createARAFactory<AraDocumentController>();
}
#endif
