#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "HarmonicNoiseSeparator.h"
#include "MelSpectrogram.h"
#include "../model/AnalysisConfigState.h"
#include "../model/TensionState.h"

class DiffSingerDecomposer
{
public:
    struct Settings
    {
        double splitFrequencyHz{1600.0};
        int melBins{128};
        int fftSize{1024};
    };

    struct AnalysisFeatures
    {
        juce::AudioBuffer<float> melSpectrogram;
        juce::Array<double> pitchCurve;
        double hopSeconds{0.0};
    };

    struct Decomposition
    {
        juce::AudioBuffer<float> harmonic;
        juce::AudioBuffer<float> noise;
        juce::AudioBuffer<float> analysisBuffer;
        juce::AudioBuffer<float> outputBuffer;
        AnalysisFeatures analysis;
    };

    explicit DiffSingerDecomposer(Settings settingsIn = {});

    const Settings& getSettings() const;
    void setSettings(Settings settingsIn);

    Decomposition decompose(const juce::AudioBuffer<float>& audio,
                            double sampleRate,
                            const AnalysisConfigState& config,
                            const TensionState::Blend& blend,
                            const HarmonicNoiseSeparator::OutputGains& gains) const;

    void refreshAnalysis(Decomposition& decomposition,
                         double sampleRate,
                         const AnalysisConfigState& config,
                         const TensionState::Blend& blend) const;

    void refreshOutput(Decomposition& decomposition,
                       const HarmonicNoiseSeparator::OutputGains& gains) const;

private:
    static juce::AudioBuffer<float> recombineBuffers(const juce::AudioBuffer<float>& harmonic,
                                                     const juce::AudioBuffer<float>& noise,
                                                     const HarmonicNoiseSeparator::OutputGains& gains);

    static juce::AudioBuffer<float> extractMono(const juce::AudioBuffer<float>& audio);
    AnalysisFeatures analyzeBuffer(const juce::AudioBuffer<float>& audio,
                                   double sampleRate,
                                   const AnalysisConfigState& config) const;
    juce::AudioBuffer<float> splitLow(const juce::AudioBuffer<float>& audio, double sampleRate) const;
    juce::AudioBuffer<float> splitHigh(const juce::AudioBuffer<float>& audio, double sampleRate) const;

    Settings settings;
};
