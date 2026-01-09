#pragma once

#include "MidiExtractor.h"

#include <juce_audio_basics/juce_audio_basics.h>

class SomeMidiExtractor final : public MidiExtractor
{
public:
    juce::Array<TrackInfo> getTracks(const juce::File& midiFile) override;
    std::vector<MidiNoteState> extractNotes(const juce::File& midiFile, int trackIndex) override;

private:
    bool loadMidiFile(const juce::File& midiFile, juce::MidiFile& output) const;
    static juce::String findTrackName(const juce::MidiMessageSequence& sequence);
    static double ticksToBeats(double ticks, int timeFormat);
};
