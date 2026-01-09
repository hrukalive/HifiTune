#pragma once

#include <vector>

#include <juce_core/juce_core.h>

#include "../model/MidiNoteState.h"

class MidiExtractor
{
public:
    struct TrackInfo
    {
        int index = 0;
        juce::String name;
        int noteCount = 0;
    };

    virtual ~MidiExtractor() = default;

    virtual juce::Array<TrackInfo> getTracks(const juce::File& midiFile) = 0;
    virtual std::vector<MidiNoteState> extractNotes(const juce::File& midiFile, int trackIndex) = 0;
};
