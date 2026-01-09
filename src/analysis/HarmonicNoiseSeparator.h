#pragma once

#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_data_structures/juce_data_structures.h>

#include "AudioBufferSource.h"
#include "../model/AnalysisConfigState.h"
#include "../model/SegmentState.h"
#include "../model/TensionState.h"

class HarmonicNoiseSeparator
{
public:
    struct OutputGains
    {
        double harmonic{1.0};
        double noise{1.0};
    };

    virtual ~HarmonicNoiseSeparator() = default;

    virtual juce::ValueTree analyze(const AudioBufferSource& audioSource,
                                    const AnalysisConfigState& config,
                                    const std::vector<SegmentState>& segments,
                                    double beatsPerSecond,
                                    const TensionState& tension,
                                    juce::UndoManager* undoManager) = 0;

    void setOutputGains(OutputGains gains);
    OutputGains getOutputGains() const;

    static juce::AudioBuffer<float> recombine(const juce::AudioBuffer<float>& harmonic,
                                              const juce::AudioBuffer<float>& noise,
                                              const OutputGains& gains);

    static juce::ValueTree createResultTree();
    static bool isResultTree(const juce::ValueTree& tree);

    static juce::ValueTree createSegmentsTree();
    static bool isSegmentsTree(const juce::ValueTree& tree);

    static juce::ValueTree createSegmentTree(double startBeat, double lengthBeats);
    static bool isSegmentTree(const juce::ValueTree& tree);

    static double getSegmentStartBeat(const juce::ValueTree& tree);
    static double getSegmentLengthBeats(const juce::ValueTree& tree);
    static void setSegmentStartBeat(juce::ValueTree& tree, double startBeat, juce::UndoManager* undoManager);
    static void setSegmentLengthBeats(juce::ValueTree& tree, double lengthBeats, juce::UndoManager* undoManager);

    static juce::Array<double> getHarmonicMask(const juce::ValueTree& tree);
    static juce::Array<double> getNoiseMask(const juce::ValueTree& tree);
    static void setHarmonicMask(juce::ValueTree& tree, const juce::Array<double>& values, juce::UndoManager* undoManager);
    static void setNoiseMask(juce::ValueTree& tree, const juce::Array<double>& values, juce::UndoManager* undoManager);

    static OutputGains getSegmentGains(const juce::ValueTree& tree);
    static void setSegmentGains(juce::ValueTree& tree, OutputGains gains, juce::UndoManager* undoManager);

    static const juce::Identifier resultTypeId;
    static const juce::Identifier segmentsTypeId;
    static const juce::Identifier segmentTypeId;
    static const juce::Identifier segmentStartBeatId;
    static const juce::Identifier segmentLengthBeatsId;
    static const juce::Identifier harmonicMaskId;
    static const juce::Identifier noiseMaskId;
    static const juce::Identifier harmonicGainId;
    static const juce::Identifier noiseGainId;

protected:
    OutputGains outputGains{};
};
