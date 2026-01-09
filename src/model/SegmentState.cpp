#include "SegmentState.h"

const juce::Identifier SegmentState::typeId{"Segment"};
const juce::Identifier SegmentState::startBeatId{"startBeat"};
const juce::Identifier SegmentState::lengthBeatsId{"lengthBeats"};

SegmentState::SegmentState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

SegmentState SegmentState::create(double startBeat, double lengthBeats)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(startBeatId, startBeat, nullptr);
    tree.setProperty(lengthBeatsId, lengthBeats, nullptr);
    return SegmentState(tree);
}

bool SegmentState::isSegment(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool SegmentState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree SegmentState::getValueTree() const
{
    return state;
}

double SegmentState::getStartBeat() const
{
    return static_cast<double>(state.getProperty(startBeatId));
}

double SegmentState::getLengthBeats() const
{
    return static_cast<double>(state.getProperty(lengthBeatsId));
}

void SegmentState::setStartBeat(double startBeat, juce::UndoManager* undoManager)
{
    state.setProperty(startBeatId, startBeat, undoManager);
}

void SegmentState::setLengthBeats(double lengthBeats, juce::UndoManager* undoManager)
{
    state.setProperty(lengthBeatsId, lengthBeats, undoManager);
}

void SegmentState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void SegmentState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
