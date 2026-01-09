#pragma once

#include <juce_data_structures/juce_data_structures.h>

class AnalysisConfigState
{
public:
    explicit AnalysisConfigState(juce::ValueTree tree);

    static AnalysisConfigState create(double samplingRate, int hopSize);
    static bool isAnalysisConfig(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    double getSamplingRate() const;
    int getHopSize() const;

    void setSamplingRate(double samplingRate, juce::UndoManager* undoManager);
    void setHopSize(int hopSize, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier samplingRateId;
    static const juce::Identifier hopSizeId;

private:
    juce::ValueTree state;
};
