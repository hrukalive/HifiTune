#include "NoteState.h"

const juce::Identifier NoteState::typeId{"Note"};
const juce::Identifier NoteState::startBeatId{"startBeat"};
const juce::Identifier NoteState::lengthBeatsId{"lengthBeats"};
const juce::Identifier NoteState::midiNoteId{"midiNote"};
const juce::Identifier NoteState::velocityId{"velocity"};

NoteState::NoteState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

NoteState NoteState::create(double startBeat, double lengthBeats, int midiNote)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(startBeatId, startBeat, nullptr);
    tree.setProperty(lengthBeatsId, lengthBeats, nullptr);
    tree.setProperty(midiNoteId, midiNote, nullptr);
    tree.setProperty(velocityId, 1.0, nullptr);
    return NoteState(tree);
}

bool NoteState::isNote(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool NoteState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree NoteState::getValueTree() const
{
    return state;
}

double NoteState::getStartBeat() const
{
    return static_cast<double>(state.getProperty(startBeatId));
}

double NoteState::getLengthBeats() const
{
    return static_cast<double>(state.getProperty(lengthBeatsId));
}

int NoteState::getMidiNote() const
{
    return static_cast<int>(state.getProperty(midiNoteId));
}

double NoteState::getVelocity() const
{
    return static_cast<double>(state.getProperty(velocityId));
}

void NoteState::setStartBeat(double startBeat, juce::UndoManager* undoManager)
{
    state.setProperty(startBeatId, startBeat, undoManager);
}

void NoteState::setLengthBeats(double lengthBeats, juce::UndoManager* undoManager)
{
    state.setProperty(lengthBeatsId, lengthBeats, undoManager);
}

void NoteState::setMidiNote(int midiNote, juce::UndoManager* undoManager)
{
    state.setProperty(midiNoteId, midiNote, undoManager);
}

void NoteState::setVelocity(double velocity, juce::UndoManager* undoManager)
{
    state.setProperty(velocityId, velocity, undoManager);
}

void NoteState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void NoteState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
