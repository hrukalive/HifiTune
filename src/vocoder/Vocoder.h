#pragma once

#include <map>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

class Vocoder
{
public:
    struct RenderSettings
    {
        int hopSizeOverride { 0 };
        double hopSecondsOverride { 0.0 };
        double crossfadeSeconds { 0.015 };
        double extendSeconds { 0.0 };
        bool replaceWholeSegment { false };
    };

    struct RenderInput
    {
        juce::AudioBuffer<float> rawAudio;
        double sampleRate { 0.0 };
        juce::AudioBuffer<float> melSpectrogram;
        juce::Array<double> pitchCurve;
        double hopSeconds { 0.0 };
        juce::Array<juce::Range<int>> editedRegions;
        juce::String cacheKey;
    };

    struct RenderOutput
    {
        juce::AudioBuffer<float> audio;
    };

    virtual ~Vocoder() = default;

    virtual bool isValid() const = 0;

    RenderOutput render(const RenderInput& input, const RenderSettings& settings);
    const juce::AudioBuffer<float>* getCachedRender(const juce::String& cacheKey) const;

protected:
    virtual juce::AudioBuffer<float> synthesize(const RenderInput& input,
                                                const RenderSettings& settings,
                                                int hopSize) = 0;

    static int resolveHopSize(const RenderInput& input, const RenderSettings& settings);
    static juce::Array<juce::Range<int>> normalizeEditedRegions(const juce::Array<juce::Range<int>>& regions,
                                                                int totalSamples,
                                                                int extendSamples);
    static void applyBlend(juce::AudioBuffer<float>& output,
                           const juce::AudioBuffer<float>& raw,
                           const juce::AudioBuffer<float>& synth,
                           const juce::Array<juce::Range<int>>& regions,
                           int crossfadeSamples);

private:
    mutable juce::CriticalSection cacheMutex;
    std::map<juce::String, juce::AudioBuffer<float>> renderCache;
};
