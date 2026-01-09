#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "../model/ClipState.h"
#include "../model/ProjectState.h"
#include "../model/SegmentState.h"
#include "../model/TrackState.h"

class AraRegionMapper
{
public:
    struct RegionInfo
    {
        juce::String regionKey;
        double startBeat = 0.0;
        double lengthBeats = 0.0;
        const juce::AudioBuffer<float>* audioBuffer = nullptr;
        double sampleRate = 0.0;
    };

    explicit AraRegionMapper(ProjectState& projectState);

    ClipState mapRegionToClip(const RegionInfo& info, juce::UndoManager* undoManager);
    void rebuildSegmentsForClip(const RegionInfo& info, juce::UndoManager* undoManager);
    void removeRegion(const juce::String& regionKey, juce::UndoManager* undoManager);
    void syncTrack(const juce::String& trackKey, const juce::String& name, juce::UndoManager* undoManager);

    static const juce::Identifier araRegionKeyId;
    static const juce::Identifier araTrackKeyId;
    static const juce::Identifier segmentClipKeyId;

private:
    juce::ValueTree findClipTreeForRegion(const juce::String& regionKey) const;
    juce::ValueTree findTrackTreeForKey(const juce::String& trackKey) const;
    void removeSegmentsForClip(const juce::String& regionKey, juce::UndoManager* undoManager);

    std::vector<std::pair<int, int>> detectNonSilentSections(const juce::AudioBuffer<float>& buffer,
                                                             float thresholdDb,
                                                             int minSilenceSamples) const;

    ProjectState& projectState;
};
