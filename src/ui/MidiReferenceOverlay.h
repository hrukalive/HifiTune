#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../model/MidiNoteState.h"

class MidiReferenceOverlay final : public juce::Component
{
public:
    void setReferenceNotes(std::vector<MidiNoteState> notes);
    void setViewRange(double startBeat, double endBeat, int lowestMidiNote, int highestMidiNote);
    void setOverlayColour(juce::Colour colour);

    void paint(juce::Graphics& g) override;

private:
    std::vector<MidiNoteState> referenceNotes;
    double viewStartBeat = 0.0;
    double viewEndBeat = 1.0;
    int viewLowestNote = 48;
    int viewHighestNote = 72;
    juce::Colour overlayColour = juce::Colour::fromString("#6EA8FF").withAlpha(0.35f);
};
