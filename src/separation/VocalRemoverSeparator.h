#pragma once

#include <map>

#include "DiffSingerDecomposer.h"
#include "HarmonicNoiseSeparator.h"

class VocalRemoverSeparator final : public HarmonicNoiseSeparator
{
public:
    VocalRemoverSeparator();

    juce::ValueTree analyze(const AudioBufferSource& audioSource,
                            const AnalysisConfigState& config,
                            const std::vector<SegmentState>& segments,
                            double beatsPerSecond,
                            const TensionState& tension,
                            juce::UndoManager* undoManager) override;

    void setDecomposerSettings(const DiffSingerDecomposer::Settings& settings);

private:
    struct SegmentKey
    {
        double startBeat{0.0};
        double lengthBeats{0.0};

        bool operator<(const SegmentKey& other) const;
    };

    struct CachedSegment
    {
        DiffSingerDecomposer::Decomposition decomposition;
        juce::Array<double> harmonicMask;
        juce::Array<double> noiseMask;
        TensionState::Blend analysisBlend{};
        HarmonicNoiseSeparator::OutputGains outputGains{};
    };

    juce::String makeSettingsSignature(const AnalysisConfigState& config, double sampleRate) const;
    void computeMasks(const juce::AudioBuffer<float>& harmonic,
                      const juce::AudioBuffer<float>& noise,
                      int hopSize,
                      juce::Array<double>& harmonicMask,
                      juce::Array<double>& noiseMask) const;

    DiffSingerDecomposer decomposer;
    juce::String settingsSignature;
    std::map<SegmentKey, CachedSegment> cache;
};
