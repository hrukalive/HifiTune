#include "PitchAnalyzer.h"

namespace
{
juce::var toVarArray(const juce::Array<double>& values)
{
    juce::Array<juce::var> vars;
    vars.ensureStorageAllocated(values.size());
    for (double value : values)
        vars.add(value);
    return vars;
}

juce::Array<double> fromVarArray(const juce::var& value)
{
    juce::Array<double> result;
    if (const auto* array = value.getArray())
    {
        result.ensureStorageAllocated(array->size());
        for (const auto& item : *array)
            result.add(static_cast<double>(item));
    }
    return result;
}
}

const juce::Identifier PitchAnalyzer::resultTypeId{"PitchAnalysisResult"};
const juce::Identifier PitchAnalyzer::f0CurveId{"f0Curve"};

juce::ValueTree PitchAnalyzer::createResultTree()
{
    return juce::ValueTree(resultTypeId);
}

bool PitchAnalyzer::isResultTree(const juce::ValueTree& tree)
{
    return tree.hasType(resultTypeId);
}

juce::Array<double> PitchAnalyzer::getF0Curve(const juce::ValueTree& tree)
{
    return fromVarArray(tree.getProperty(f0CurveId));
}

void PitchAnalyzer::setF0Curve(juce::ValueTree& tree,
                               const juce::Array<double>& values,
                               juce::UndoManager* undoManager)
{
    tree.setProperty(f0CurveId, toVarArray(values), undoManager);
}
