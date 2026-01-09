#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "../model/PitchCurveState.h"

struct PitchDetectionResult
{
    juce::Array<double> f0Curve;
    juce::Array<float> confidence;
    double hopSeconds { 0.0 };
};

class PitchDetector
{
public:
    virtual ~PitchDetector() = default;

    virtual PitchDetectionResult detect(const juce::AudioBuffer<float>& audio, double sampleRate) = 0;

    static PitchCurveState toPitchCurveState(const PitchDetectionResult& result,
                                             juce::UndoManager* undoManager);
    static float computeTuningConfidence(double f0Hz, float modelConfidence);
};
