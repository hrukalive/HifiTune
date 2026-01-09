#include "AraRegionMapper.h"

#include <algorithm>
#include <cmath>

const juce::Identifier AraRegionMapper::araRegionKeyId{"araRegionKey"};
const juce::Identifier AraRegionMapper::araTrackKeyId{"araTrackKey"};
const juce::Identifier AraRegionMapper::segmentClipKeyId{"segmentClipKey"};

AraRegionMapper::AraRegionMapper(ProjectState& projectStateIn)
    : projectState(projectStateIn)
{
}

ClipState AraRegionMapper::mapRegionToClip(const RegionInfo& info, juce::UndoManager* undoManager)
{
    auto clipsTree = projectState.getClipsTree();
    auto clipTree = findClipTreeForRegion(info.regionKey);

    if (!clipTree.isValid())
    {
        auto clipState = ClipState::create(info.startBeat, info.lengthBeats);
        clipTree = clipState.getValueTree();
        clipTree.setProperty(araRegionKeyId, info.regionKey, nullptr);
        clipsTree.addChild(clipTree, -1, undoManager);
    }

    ClipState clipState(clipTree);
    clipState.setStartBeat(info.startBeat, undoManager);
    clipState.setLengthBeats(info.lengthBeats, undoManager);
    return clipState;
}

void AraRegionMapper::rebuildSegmentsForClip(const RegionInfo& info, juce::UndoManager* undoManager)
{
    removeSegmentsForClip(info.regionKey, undoManager);

    if (info.lengthBeats <= 0.0)
        return;

    auto segmentsTree = projectState.getSegmentsTree();

    if (info.audioBuffer == nullptr || info.audioBuffer->getNumSamples() == 0)
    {
        auto segment = SegmentState::create(info.startBeat, info.lengthBeats);
        auto segmentTree = segment.getValueTree();
        segmentTree.setProperty(segmentClipKeyId, info.regionKey, nullptr);
        segmentsTree.addChild(segmentTree, -1, undoManager);
        return;
    }

    const auto totalSamples = info.audioBuffer->getNumSamples();
    const auto beatsPerSample = (totalSamples > 0 ? info.lengthBeats / static_cast<double>(totalSamples) : 0.0);
    const auto minSilenceSamples = info.sampleRate > 0.0 ? static_cast<int>(info.sampleRate * 0.1) : 0;

    const auto sections = detectNonSilentSections(*info.audioBuffer, -45.0f, minSilenceSamples);
    if (sections.empty() || beatsPerSample <= 0.0)
    {
        auto segment = SegmentState::create(info.startBeat, info.lengthBeats);
        auto segmentTree = segment.getValueTree();
        segmentTree.setProperty(segmentClipKeyId, info.regionKey, nullptr);
        segmentsTree.addChild(segmentTree, -1, undoManager);
        return;
    }

    for (const auto& [startSample, endSample] : sections)
    {
        const auto segmentStart = info.startBeat + static_cast<double>(startSample) * beatsPerSample;
        const auto segmentLength = static_cast<double>(endSample - startSample) * beatsPerSample;
        if (segmentLength <= 0.0)
            continue;

        auto segment = SegmentState::create(segmentStart, segmentLength);
        auto segmentTree = segment.getValueTree();
        segmentTree.setProperty(segmentClipKeyId, info.regionKey, nullptr);
        segmentsTree.addChild(segmentTree, -1, undoManager);
    }
}

void AraRegionMapper::removeRegion(const juce::String& regionKey, juce::UndoManager* undoManager)
{
    auto clipsTree = projectState.getClipsTree();
    auto clipTree = findClipTreeForRegion(regionKey);
    if (clipTree.isValid())
        clipsTree.removeChild(clipTree, undoManager);

    removeSegmentsForClip(regionKey, undoManager);
}

void AraRegionMapper::syncTrack(const juce::String& trackKey, const juce::String& name, juce::UndoManager* undoManager)
{
    auto tracksTree = projectState.getTracksTree();
    auto trackTree = findTrackTreeForKey(trackKey);

    if (!trackTree.isValid())
    {
        auto trackState = TrackState::create(name);
        trackTree = trackState.getValueTree();
        trackTree.setProperty(araTrackKeyId, trackKey, nullptr);
        tracksTree.addChild(trackTree, -1, undoManager);
    }

    TrackState trackState(trackTree);
    trackState.setName(name, undoManager);
}

juce::ValueTree AraRegionMapper::findClipTreeForRegion(const juce::String& regionKey) const
{
    auto clipsTree = projectState.getClipsTree();
    for (int i = 0; i < clipsTree.getNumChildren(); ++i)
    {
        auto child = clipsTree.getChild(i);
        if (child.getProperty(araRegionKeyId).toString() == regionKey)
            return child;
    }
    return {};
}

juce::ValueTree AraRegionMapper::findTrackTreeForKey(const juce::String& trackKey) const
{
    auto tracksTree = projectState.getTracksTree();
    for (int i = 0; i < tracksTree.getNumChildren(); ++i)
    {
        auto child = tracksTree.getChild(i);
        if (child.getProperty(araTrackKeyId).toString() == trackKey)
            return child;
    }
    return {};
}

void AraRegionMapper::removeSegmentsForClip(const juce::String& regionKey, juce::UndoManager* undoManager)
{
    auto segmentsTree = projectState.getSegmentsTree();
    for (int i = segmentsTree.getNumChildren(); --i >= 0;)
    {
        auto child = segmentsTree.getChild(i);
        if (child.getProperty(segmentClipKeyId).toString() == regionKey)
            segmentsTree.removeChild(child, undoManager);
    }
}

std::vector<std::pair<int, int>> AraRegionMapper::detectNonSilentSections(const juce::AudioBuffer<float>& buffer,
                                                                          float thresholdDb,
                                                                          int minSilenceSamples) const
{
    std::vector<std::pair<int, int>> sections;
    if (buffer.getNumSamples() == 0)
        return sections;

    const auto threshold = std::pow(10.0f, thresholdDb / 20.0f);
    const auto totalSamples = buffer.getNumSamples();

    bool inSegment = false;
    int segmentStart = 0;
    int silentSamples = 0;

    for (int sample = 0; sample < totalSamples; ++sample)
    {
        float maxAbs = 0.0f;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            maxAbs = std::max(maxAbs, std::abs(buffer.getSample(channel, sample)));

        if (maxAbs > threshold)
        {
            if (!inSegment)
            {
                inSegment = true;
                segmentStart = sample;
            }
            silentSamples = 0;
        }
        else if (inSegment)
        {
            ++silentSamples;
            if (silentSamples >= minSilenceSamples)
            {
                const auto segmentEnd = sample - silentSamples;
                if (segmentEnd > segmentStart)
                    sections.emplace_back(segmentStart, segmentEnd);
                inSegment = false;
            }
        }
    }

    if (inSegment)
        sections.emplace_back(segmentStart, totalSamples);

    return sections;
}
