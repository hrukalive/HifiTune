#include "PitchCurveState.h"

const juce::Identifier PitchCurveState::typeId{"PitchCurve"};
const juce::Identifier PitchCurveState::pointsId{"points"};
const juce::Identifier PitchCurveState::confidenceId{"confidence"};

PitchCurveState::PitchCurveState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

PitchCurveState PitchCurveState::create()
{
    juce::ValueTree tree(typeId);
    return PitchCurveState(tree);
}

bool PitchCurveState::isPitchCurve(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool PitchCurveState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree PitchCurveState::getValueTree() const
{
    return state;
}

juce::var PitchCurveState::getPoints() const
{
    return state.getProperty(pointsId);
}

void PitchCurveState::setPoints(const juce::var& points, juce::UndoManager* undoManager)
{
    state.setProperty(pointsId, points, undoManager);
}

juce::var PitchCurveState::getConfidence() const
{
    return state.getProperty(confidenceId);
}

void PitchCurveState::setConfidence(const juce::var& confidence, juce::UndoManager* undoManager)
{
    state.setProperty(confidenceId, confidence, undoManager);
}

void PitchCurveState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void PitchCurveState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
