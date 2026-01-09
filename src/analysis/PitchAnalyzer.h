#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "AudioBufferSource.h"
#include "../model/AnalysisConfigState.h"

class PitchAnalyzer
{
public:
    virtual ~PitchAnalyzer() = default;

    virtual juce::ValueTree analyze(const AudioBufferSource& audioSource,
                                    const AnalysisConfigState& config,
                                    juce::UndoManager* undoManager) = 0;

    static juce::ValueTree createResultTree();
    static bool isResultTree(const juce::ValueTree& tree);

    static juce::Array<double> getF0Curve(const juce::ValueTree& tree);
    static void setF0Curve(juce::ValueTree& tree, const juce::Array<double>& values, juce::UndoManager* undoManager);

    static const juce::Identifier resultTypeId;
    static const juce::Identifier f0CurveId;
};
