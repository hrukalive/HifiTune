#include "AnalysisConfigState.h"

const juce::Identifier AnalysisConfigState::typeId{"AnalysisConfig"};
const juce::Identifier AnalysisConfigState::samplingRateId{"samplingRate"};
const juce::Identifier AnalysisConfigState::hopSizeId{"hopSize"};

AnalysisConfigState::AnalysisConfigState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

AnalysisConfigState AnalysisConfigState::create(double samplingRate, int hopSize)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(samplingRateId, samplingRate, nullptr);
    tree.setProperty(hopSizeId, hopSize, nullptr);
    return AnalysisConfigState(tree);
}

bool AnalysisConfigState::isAnalysisConfig(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool AnalysisConfigState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree AnalysisConfigState::getValueTree() const
{
    return state;
}

double AnalysisConfigState::getSamplingRate() const
{
    return static_cast<double>(state.getProperty(samplingRateId));
}

int AnalysisConfigState::getHopSize() const
{
    return static_cast<int>(state.getProperty(hopSizeId));
}

void AnalysisConfigState::setSamplingRate(double samplingRate, juce::UndoManager* undoManager)
{
    state.setProperty(samplingRateId, samplingRate, undoManager);
}

void AnalysisConfigState::setHopSize(int hopSize, juce::UndoManager* undoManager)
{
    state.setProperty(hopSizeId, hopSize, undoManager);
}

void AnalysisConfigState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void AnalysisConfigState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
