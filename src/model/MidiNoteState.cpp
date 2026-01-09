#include "MidiNoteState.h"

const juce::Identifier MidiNoteState::typeId{"MidiNote"};
const juce::Identifier MidiNoteState::startBeatId{"startBeat"};
const juce::Identifier MidiNoteState::lengthBeatsId{"lengthBeats"};
const juce::Identifier MidiNoteState::midiNoteId{"midiNote"};
const juce::Identifier MidiNoteState::velocityId{"velocity"};

MidiNoteState::MidiNoteState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

MidiNoteState MidiNoteState::create(double startBeat, double lengthBeats, int midiNote, double velocity)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(startBeatId, startBeat, nullptr);
    tree.setProperty(lengthBeatsId, lengthBeats, nullptr);
    tree.setProperty(midiNoteId, midiNote, nullptr);
    tree.setProperty(velocityId, velocity, nullptr);
    return MidiNoteState(tree);
}

bool MidiNoteState::isNote(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool MidiNoteState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree MidiNoteState::getValueTree() const
{
    return state;
}

double MidiNoteState::getStartBeat() const
{
    return static_cast<double>(state.getProperty(startBeatId));
}

double MidiNoteState::getLengthBeats() const
{
    return static_cast<double>(state.getProperty(lengthBeatsId));
}

int MidiNoteState::getMidiNote() const
{
    return static_cast<int>(state.getProperty(midiNoteId));
}

double MidiNoteState::getVelocity() const
{
    return static_cast<double>(state.getProperty(velocityId));
}

void MidiNoteState::setStartBeat(double startBeat, juce::UndoManager* undoManager)
{
    state.setProperty(startBeatId, startBeat, undoManager);
}

void MidiNoteState::setLengthBeats(double lengthBeats, juce::UndoManager* undoManager)
{
    state.setProperty(lengthBeatsId, lengthBeats, undoManager);
}

void MidiNoteState::setMidiNote(int midiNote, juce::UndoManager* undoManager)
{
    state.setProperty(midiNoteId, midiNote, undoManager);
}

void MidiNoteState::setVelocity(double velocity, juce::UndoManager* undoManager)
{
    state.setProperty(velocityId, velocity, undoManager);
}

void MidiNoteState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void MidiNoteState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
