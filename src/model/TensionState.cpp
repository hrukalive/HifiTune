#include "TensionState.h"

const juce::Identifier TensionState::typeId{"Tension"};
const juce::Identifier TensionState::pointsId{"points"};

TensionState::TensionState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

TensionState TensionState::create()
{
    juce::ValueTree tree(typeId);
    return TensionState(tree);
}

bool TensionState::isTension(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool TensionState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree TensionState::getValueTree() const
{
    return state;
}

juce::var TensionState::getPoints() const
{
    return state.getProperty(pointsId);
}

void TensionState::setPoints(const juce::var& points, juce::UndoManager* undoManager)
{
    state.setProperty(pointsId, points, undoManager);
}

void TensionState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void TensionState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
