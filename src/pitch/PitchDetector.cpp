#include "PitchDetector.h"

#include <cmath>

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

juce::var toVarArray(const juce::Array<float>& values)
{
    juce::Array<juce::var> vars;
    vars.ensureStorageAllocated(values.size());
    for (float value : values)
        vars.add(static_cast<double>(value));
    return vars;
}
}

PitchCurveState PitchDetector::toPitchCurveState(const PitchDetectionResult& result,
                                                 juce::UndoManager* undoManager)
{
    auto curve = PitchCurveState::create();
    curve.setPoints(toVarArray(result.f0Curve), undoManager);
    curve.setConfidence(toVarArray(result.confidence), undoManager);
    return curve;
}

float PitchDetector::computeTuningConfidence(double f0Hz, float modelConfidence)
{
    if (f0Hz <= 0.0)
        return 0.0f;

    const double midi = 69.0 + 12.0 * std::log2(f0Hz / 440.0);
    const double nearestMidi = std::round(midi);
    const double nearestHz = 440.0 * std::pow(2.0, (nearestMidi - 69.0) / 12.0);
    const double cents = 1200.0 * std::log2(f0Hz / nearestHz);
    const double tuningConfidence = 1.0 - juce::jlimit(0.0, 1.0, std::abs(cents) / 50.0);

    return static_cast<float>(juce::jlimit(0.0, 1.0, tuningConfidence * modelConfidence));
}
