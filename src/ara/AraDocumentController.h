#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cstdint>
#include <optional>

#include "AraRegionMapper.h"
#include "../model/ProjectState.h"

class LocalAudioRecorder
{
public:
    void prepare(double sampleRate, int numChannels, int maxSamplesPerBlock, double durationSeconds = 30.0);
    void reset();
    void pushBlock(const juce::AudioBuffer<float>& buffer);

    const juce::AudioBuffer<float>& getBuffer() const noexcept { return recordedBuffer; }
    double getSampleRate() const noexcept { return sampleRate; }
    int64_t getWritePosition() const noexcept { return writePosition; }

private:
    juce::AudioBuffer<float> recordedBuffer;
    double sampleRate = 0.0;
    int64_t writePosition = 0;
};

#if HIFITUNE_ENABLE_ARA
class AraDocumentController final : public juce::ARADocumentControllerSpecialisation
{
public:
    AraDocumentController(const ARA::PlugIn::PlugInEntry* entry,
                          const ARA::ARADocumentControllerHostInstance* instance);

    const ProjectState& getProjectState() const noexcept { return projectState; }

    bool isFollowHostCursorEnabled() const;
    bool isScrollToClipEnabled() const;
    void setFollowHostCursorEnabled(bool enabled);
    void setScrollToClipEnabled(bool enabled);

    void updateTransportFromHost(const juce::AudioPlayHead::PositionInfo& positionInfo);

    static juce::String createStableId(const void* pointer);

protected:
    juce::ARAPlaybackRenderer* doCreatePlaybackRenderer() noexcept override;
    bool doRestoreObjectsFromStream(juce::ARAInputStream& input,
                                    const juce::ARARestoreObjectsFilter* filter) noexcept override;
    bool doStoreObjectsToStream(juce::ARAOutputStream& output,
                                const juce::ARAStoreObjectsFilter* filter) noexcept override;

    void willBeginEditing(juce::ARADocument* document) override;
    void didEndEditing(juce::ARADocument* document) override;
    void didUpdateRegionSequenceProperties(juce::ARARegionSequence* regionSequence) override;
    void didAddPlaybackRegionToRegionSequence(juce::ARARegionSequence* regionSequence,
                                              juce::ARAPlaybackRegion* playbackRegion) override;
    void willRemovePlaybackRegionFromRegionSequence(juce::ARARegionSequence* regionSequence,
                                                    juce::ARAPlaybackRegion* playbackRegion) override;
    void didUpdatePlaybackRegionProperties(juce::ARAPlaybackRegion* playbackRegion) override;

private:
    class AraPlaybackRenderer;

    void ensureSettingsDefaults();
    void updatePlaybackRegion(juce::ARAPlaybackRegion* playbackRegion);

    juce::UndoManager* currentUndoManager();

    ProjectState projectState;
    AraRegionMapper regionMapper{projectState};
    std::optional<ProjectState::Transaction> activeTransaction;

    juce::AudioPlayHead::PositionInfo lastPosition;

    static const juce::Identifier followHostCursorId;
    static const juce::Identifier scrollToClipId;
    static const juce::Identifier playbackTimeSecondsId;
    static const juce::Identifier transportIsPlayingId;
    static const juce::Identifier transportIsRecordingId;
    static const juce::Identifier transportIsLoopingId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AraDocumentController)
};
#endif
