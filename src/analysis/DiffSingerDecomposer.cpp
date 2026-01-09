#include "DiffSingerDecomposer.h"

#include <algorithm>
#include <cmath>

#include <juce_dsp/juce_dsp.h>

DiffSingerDecomposer::DiffSingerDecomposer(Settings settingsIn)
    : settings(settingsIn)
{
}

const DiffSingerDecomposer::Settings& DiffSingerDecomposer::getSettings() const
{
    return settings;
}

void DiffSingerDecomposer::setSettings(Settings settingsIn)
{
    settings = settingsIn;
}

DiffSingerDecomposer::Decomposition DiffSingerDecomposer::decompose(const juce::AudioBuffer<float>& audio,
                                                                    double sampleRate,
                                                                    const AnalysisConfigState& config,
                                                                    const TensionState::Blend& blend,
                                                                    const HarmonicNoiseSeparator::OutputGains& gains) const
{
    Decomposition result;
    if (audio.getNumSamples() == 0)
        return result;

    result.harmonic = splitLow(audio, sampleRate);
    result.noise = splitHigh(audio, sampleRate);

    refreshAnalysis(result, sampleRate, config, blend);
    refreshOutput(result, gains);

    return result;
}

void DiffSingerDecomposer::refreshAnalysis(Decomposition& decomposition,
                                           double sampleRate,
                                           const AnalysisConfigState& config,
                                           const TensionState::Blend& blend) const
{
    HarmonicNoiseSeparator::OutputGains blendGains{ blend.harmonic, blend.noise };
    decomposition.analysisBuffer = recombineBuffers(decomposition.harmonic, decomposition.noise, blendGains);
    decomposition.analysis = analyzeBuffer(decomposition.analysisBuffer, sampleRate, config);
}

void DiffSingerDecomposer::refreshOutput(Decomposition& decomposition,
                                         const HarmonicNoiseSeparator::OutputGains& gains) const
{
    decomposition.outputBuffer = recombineBuffers(decomposition.harmonic, decomposition.noise, gains);
}

juce::AudioBuffer<float> DiffSingerDecomposer::recombineBuffers(const juce::AudioBuffer<float>& harmonic,
                                                                const juce::AudioBuffer<float>& noise,
                                                                const HarmonicNoiseSeparator::OutputGains& gains)
{
    return HarmonicNoiseSeparator::recombine(harmonic, noise, gains);
}

juce::AudioBuffer<float> DiffSingerDecomposer::extractMono(const juce::AudioBuffer<float>& audio)
{
    if (audio.getNumSamples() == 0)
        return {};

    juce::AudioBuffer<float> mono(1, audio.getNumSamples());
    mono.clear();

    for (int sample = 0; sample < audio.getNumSamples(); ++sample)
    {
        float sum = 0.0f;
        for (int channel = 0; channel < audio.getNumChannels(); ++channel)
            sum += audio.getSample(channel, sample);
        mono.setSample(0, sample, sum / static_cast<float>(audio.getNumChannels()));
    }

    return mono;
}

DiffSingerDecomposer::AnalysisFeatures DiffSingerDecomposer::analyzeBuffer(const juce::AudioBuffer<float>& audio,
                                                                           double sampleRate,
                                                                           const AnalysisConfigState& config) const
{
    AnalysisFeatures features;
    if (audio.getNumSamples() == 0 || sampleRate <= 0.0)
        return features;

    const int hopSize = config.getHopSize() > 0 ? config.getHopSize() : 512;
    const int fftSize = settings.fftSize > 0 ? settings.fftSize : hopSize * 4;

    MelSpectrogram::Settings melSettings;
    melSettings.fftSize = fftSize;
    melSettings.hopSize = hopSize;
    melSettings.melBins = settings.melBins;
    melSettings.windowType = juce::dsp::WindowingFunction<float>::hann;

    MelSpectrogram spectrogram(melSettings);
    features.melSpectrogram = spectrogram.render(audio, sampleRate);
    features.hopSeconds = static_cast<double>(hopSize) / sampleRate;

    auto mono = extractMono(audio);
    const int numSamples = mono.getNumSamples();
    if (numSamples == 0)
        return features;

    const int frames = 1 + (numSamples - 1) / hopSize;
    features.pitchCurve.ensureStorageAllocated(frames);

    for (int frame = 0; frame < frames; ++frame)
    {
        const int startSample = frame * hopSize;
        const int endSample = std::min(numSamples, startSample + fftSize);
        if (endSample <= startSample)
        {
            features.pitchCurve.add(0.0);
            continue;
        }

        int zeroCrossings = 0;
        float prev = mono.getSample(0, startSample);
        for (int sample = startSample + 1; sample < endSample; ++sample)
        {
            const float value = mono.getSample(0, sample);
            if ((prev >= 0.0f && value < 0.0f) || (prev < 0.0f && value >= 0.0f))
                ++zeroCrossings;
            prev = value;
        }

        const double duration = static_cast<double>(endSample - startSample) / sampleRate;
        const double frequency = duration > 0.0 ? (zeroCrossings / (2.0 * duration)) : 0.0;
        features.pitchCurve.add(frequency);
    }

    return features;
}

juce::AudioBuffer<float> DiffSingerDecomposer::splitLow(const juce::AudioBuffer<float>& audio, double sampleRate) const
{
    if (audio.getNumSamples() == 0)
        return {};

    juce::AudioBuffer<float> output(audio.getNumChannels(), audio.getNumSamples());
    output.clear();

    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, settings.splitFrequencyHz);
        juce::dsp::IIR::Filter<float> filter(coefficients);
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
        {
            const float value = filter.processSample(audio.getSample(channel, sample));
            output.setSample(channel, sample, value);
        }
    }

    return output;
}

juce::AudioBuffer<float> DiffSingerDecomposer::splitHigh(const juce::AudioBuffer<float>& audio, double sampleRate) const
{
    if (audio.getNumSamples() == 0)
        return {};

    juce::AudioBuffer<float> output(audio.getNumChannels(), audio.getNumSamples());
    output.clear();

    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
    {
        auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, settings.splitFrequencyHz);
        juce::dsp::IIR::Filter<float> filter(coefficients);
        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
        {
            const float value = filter.processSample(audio.getSample(channel, sample));
            output.setSample(channel, sample, value);
        }
    }

    return output;
}
