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
const juce::Identifier HarmonicNoiseSeparator::segmentsTypeId{"HarmonicNoiseSegments"};
const juce::Identifier HarmonicNoiseSeparator::segmentTypeId{"HarmonicNoiseSegment"};
const juce::Identifier HarmonicNoiseSeparator::segmentStartBeatId{"segmentStartBeat"};
const juce::Identifier HarmonicNoiseSeparator::segmentLengthBeatsId{"segmentLengthBeats"};
const juce::Identifier HarmonicNoiseSeparator::harmonicMaskId{"harmonicMask"};
const juce::Identifier HarmonicNoiseSeparator::noiseMaskId{"noiseMask"};
const juce::Identifier HarmonicNoiseSeparator::harmonicGainId{"harmonicGain"};
const juce::Identifier HarmonicNoiseSeparator::noiseGainId{"noiseGain"};

void HarmonicNoiseSeparator::setOutputGains(OutputGains gains)
{
    outputGains = gains;
}

HarmonicNoiseSeparator::OutputGains HarmonicNoiseSeparator::getOutputGains() const
{
    return outputGains;
}

juce::AudioBuffer<float> HarmonicNoiseSeparator::recombine(const juce::AudioBuffer<float>& harmonic,
                                                           const juce::AudioBuffer<float>& noise,
                                                           const OutputGains& gains)
{
    const int channels = juce::jmax(harmonic.getNumChannels(), noise.getNumChannels());
    const int samples = juce::jmax(harmonic.getNumSamples(), noise.getNumSamples());
    juce::AudioBuffer<float> output(channels, samples);
    output.clear();

    for (int channel = 0; channel < channels; ++channel)
    {
        for (int sample = 0; sample < samples; ++sample)
        {
            float value = 0.0f;
            if (channel < harmonic.getNumChannels() && sample < harmonic.getNumSamples())
                value += harmonic.getSample(channel, sample) * static_cast<float>(gains.harmonic);
            if (channel < noise.getNumChannels() && sample < noise.getNumSamples())
                value += noise.getSample(channel, sample) * static_cast<float>(gains.noise);
            output.setSample(channel, sample, value);
        }
    }

    return output;
}

juce::ValueTree HarmonicNoiseSeparator::createResultTree()
{
    return juce::ValueTree(resultTypeId);
}

bool HarmonicNoiseSeparator::isResultTree(const juce::ValueTree& tree)
{
    return tree.hasType(resultTypeId);
}

juce::ValueTree HarmonicNoiseSeparator::createSegmentsTree()
{
    return juce::ValueTree(segmentsTypeId);
}

bool HarmonicNoiseSeparator::isSegmentsTree(const juce::ValueTree& tree)
{
    return tree.hasType(segmentsTypeId);
}

juce::ValueTree HarmonicNoiseSeparator::createSegmentTree(double startBeat, double lengthBeats)
{
    juce::ValueTree tree(segmentTypeId);
    tree.setProperty(segmentStartBeatId, startBeat, nullptr);
    tree.setProperty(segmentLengthBeatsId, lengthBeats, nullptr);
    return tree;
}

bool HarmonicNoiseSeparator::isSegmentTree(const juce::ValueTree& tree)
{
    return tree.hasType(segmentTypeId);
}

double HarmonicNoiseSeparator::getSegmentStartBeat(const juce::ValueTree& tree)
{
    return static_cast<double>(tree.getProperty(segmentStartBeatId));
}

double HarmonicNoiseSeparator::getSegmentLengthBeats(const juce::ValueTree& tree)
{
    return static_cast<double>(tree.getProperty(segmentLengthBeatsId));
}

void HarmonicNoiseSeparator::setSegmentStartBeat(juce::ValueTree& tree, double startBeat, juce::UndoManager* undoManager)
{
    tree.setProperty(segmentStartBeatId, startBeat, undoManager);
}

void HarmonicNoiseSeparator::setSegmentLengthBeats(juce::ValueTree& tree, double lengthBeats, juce::UndoManager* undoManager)
{
    tree.setProperty(segmentLengthBeatsId, lengthBeats, undoManager);
}

juce::Array<double> HarmonicNoiseSeparator::getHarmonicMask(const juce::ValueTree& tree)
{
    return fromVarArray(tree.getProperty(harmonicMaskId));
}

juce::Array<double> HarmonicNoiseSeparator::getNoiseMask(const juce::ValueTree& tree)
{
    return fromVarArray(tree.getProperty(noiseMaskId));
}

void HarmonicNoiseSeparator::setHarmonicMask(juce::ValueTree& tree,
                                             const juce::Array<double>& values,
                                             juce::UndoManager* undoManager)
{
    tree.setProperty(harmonicMaskId, toVarArray(values), undoManager);
}

void HarmonicNoiseSeparator::setNoiseMask(juce::ValueTree& tree,
                                          const juce::Array<double>& values,
                                          juce::UndoManager* undoManager)
{
    tree.setProperty(noiseMaskId, toVarArray(values), undoManager);
}

HarmonicNoiseSeparator::OutputGains HarmonicNoiseSeparator::getSegmentGains(const juce::ValueTree& tree)
{
    OutputGains gains;
    if (tree.hasProperty(harmonicGainId))
        gains.harmonic = static_cast<double>(tree.getProperty(harmonicGainId));
    if (tree.hasProperty(noiseGainId))
        gains.noise = static_cast<double>(tree.getProperty(noiseGainId));
    return gains;
}

void HarmonicNoiseSeparator::setSegmentGains(juce::ValueTree& tree, OutputGains gains, juce::UndoManager* undoManager)
{
    tree.setProperty(harmonicGainId, gains.harmonic, undoManager);
    tree.setProperty(noiseGainId, gains.noise, undoManager);
}
