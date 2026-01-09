#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "AudioBufferSource.h"
#include "../model/AnalysisConfigState.h"

class HarmonicNoiseSeparator
{
public:
    virtual ~HarmonicNoiseSeparator() = default;

    virtual juce::ValueTree analyze(const AudioBufferSource& audioSource,
                                    const AnalysisConfigState& config,
                                    juce::UndoManager* undoManager) = 0;

    static juce::ValueTree createResultTree();
    static bool isResultTree(const juce::ValueTree& tree);

    static juce::Array<double> getHarmonicMask(const juce::ValueTree& tree);
    static juce::Array<double> getNoiseMask(const juce::ValueTree& tree);
    static void setHarmonicMask(const juce::ValueTree& tree, const juce::Array<double>& values, juce::UndoManager* undoManager);
    static void setNoiseMask(const juce::ValueTree& tree, const juce::Array<double>& values, juce::UndoManager* undoManager);

    static const juce::Identifier resultTypeId;
    static const juce::Identifier harmonicMaskId;
    static const juce::Identifier noiseMaskId;
};
