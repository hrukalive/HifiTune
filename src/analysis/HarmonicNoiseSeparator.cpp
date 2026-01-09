#include "HarmonicNoiseSeparator.h"

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

const juce::Identifier HarmonicNoiseSeparator::resultTypeId{"HarmonicNoiseResult"};
const juce::Identifier HarmonicNoiseSeparator::harmonicMaskId{"harmonicMask"};
const juce::Identifier HarmonicNoiseSeparator::noiseMaskId{"noiseMask"};

juce::ValueTree HarmonicNoiseSeparator::createResultTree()
{
    return juce::ValueTree(resultTypeId);
}

bool HarmonicNoiseSeparator::isResultTree(const juce::ValueTree& tree)
{
    return tree.hasType(resultTypeId);
}

juce::Array<double> HarmonicNoiseSeparator::getHarmonicMask(const juce::ValueTree& tree)
{
    return fromVarArray(tree.getProperty(harmonicMaskId));
}

juce::Array<double> HarmonicNoiseSeparator::getNoiseMask(const juce::ValueTree& tree)
{
    return fromVarArray(tree.getProperty(noiseMaskId));
}

void HarmonicNoiseSeparator::setHarmonicMask(const juce::ValueTree& tree,
                                             const juce::Array<double>& values,
                                             juce::UndoManager* undoManager)
{
    tree.setProperty(harmonicMaskId, toVarArray(values), undoManager);
}

void HarmonicNoiseSeparator::setNoiseMask(const juce::ValueTree& tree,
                                          const juce::Array<double>& values,
                                          juce::UndoManager* undoManager)
{
    tree.setProperty(noiseMaskId, toVarArray(values), undoManager);
}
