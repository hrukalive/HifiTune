#include "TrackState.h"

const juce::Identifier TrackState::typeId{"Track"};
const juce::Identifier TrackState::nameId{"name"};
const juce::Identifier TrackState::colourId{"colour"};

TrackState::TrackState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

TrackState TrackState::create(const juce::String& name)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(nameId, name, nullptr);
    return TrackState(tree);
}

bool TrackState::isTrack(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool TrackState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree TrackState::getValueTree() const
{
    return state;
}

juce::String TrackState::getName() const
{
    return state.getProperty(nameId).toString();
}

void TrackState::setName(const juce::String& name, juce::UndoManager* undoManager)
{
    state.setProperty(nameId, name, undoManager);
}

juce::var TrackState::getColour() const
{
    return state.getProperty(colourId);
}

void TrackState::setColour(const juce::var& colour, juce::UndoManager* undoManager)
{
    state.setProperty(colourId, colour, undoManager);
}

void TrackState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void TrackState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
