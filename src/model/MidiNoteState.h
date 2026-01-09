#pragma once

#include <juce_data_structures/juce_data_structures.h>

class MidiNoteState
{
public:
    explicit MidiNoteState(juce::ValueTree tree);

    static MidiNoteState create(double startBeat, double lengthBeats, int midiNote, double velocity);
    static bool isNote(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    double getStartBeat() const;
    double getLengthBeats() const;
    int getMidiNote() const;
    double getVelocity() const;

    void setStartBeat(double startBeat, juce::UndoManager* undoManager);
    void setLengthBeats(double lengthBeats, juce::UndoManager* undoManager);
    void setMidiNote(int midiNote, juce::UndoManager* undoManager);
    void setVelocity(double velocity, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier startBeatId;
    static const juce::Identifier lengthBeatsId;
    static const juce::Identifier midiNoteId;
    static const juce::Identifier velocityId;

private:
    juce::ValueTree state;
};
