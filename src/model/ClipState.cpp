#include "ClipState.h"

const juce::Identifier ClipState::typeId{"Clip"};
const juce::Identifier ClipState::startBeatId{"startBeat"};
const juce::Identifier ClipState::lengthBeatsId{"lengthBeats"};

ClipState::ClipState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

ClipState ClipState::create(double startBeat, double lengthBeats)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(startBeatId, startBeat, nullptr);
    tree.setProperty(lengthBeatsId, lengthBeats, nullptr);
    return ClipState(tree);
}

bool ClipState::isClip(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool ClipState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree ClipState::getValueTree() const
{
    return state;
}

double ClipState::getStartBeat() const
{
    return static_cast<double>(state.getProperty(startBeatId));
}

double ClipState::getLengthBeats() const
{
    return static_cast<double>(state.getProperty(lengthBeatsId));
}

void ClipState::setStartBeat(double startBeat, juce::UndoManager* undoManager)
{
    state.setProperty(startBeatId, startBeat, undoManager);
}

void ClipState::setLengthBeats(double lengthBeats, juce::UndoManager* undoManager)
{
    state.setProperty(lengthBeatsId, lengthBeats, undoManager);
}

void ClipState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void ClipState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
