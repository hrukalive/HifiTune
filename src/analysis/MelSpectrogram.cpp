#include "MelSpectrogram.h"

#include <algorithm>
#include <cmath>

namespace
{
float hzToMel(float hz)
{
    return 2595.0f * std::log10(1.0f + hz / 700.0f);
}

float melToHz(float mel)
{
    return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
}
}

MelSpectrogram::MelSpectrogram(Settings settingsIn)
    : settings(settingsIn)
{
}

const MelSpectrogram::Settings& MelSpectrogram::getSettings() const
{
    return settings;
}

void MelSpectrogram::setSettings(Settings settingsIn)
{
    settings = settingsIn;
}

juce::AudioBuffer<float> MelSpectrogram::render(const juce::AudioBuffer<float>& audio, double sampleRate) const
{
    const int fftSize = settings.fftSize;
    const int hopSize = settings.hopSize;
    const int melBins = settings.melBins;
    const int numSamples = audio.getNumSamples();

    if (fftSize <= 0 || hopSize <= 0 || melBins <= 0 || numSamples == 0)
        return {};

    const int numFrames = 1 + (numSamples - 1) / hopSize;
    juce::AudioBuffer<float> output(melBins, numFrames);
    output.clear();

    juce::dsp::FFT fft(static_cast<int>(std::log2(fftSize)));
    juce::dsp::WindowingFunction<float> window(fftSize, settings.windowType, false);

    std::vector<float> fftBuffer(static_cast<size_t>(2 * fftSize), 0.0f);
    std::vector<float> magnitude(static_cast<size_t>(fftSize / 2 + 1), 0.0f);

    const float nyquist = static_cast<float>(sampleRate / 2.0);
    const float melMin = hzToMel(0.0f);
    const float melMax = hzToMel(nyquist);

    std::vector<float> melPoints(static_cast<size_t>(melBins + 2), 0.0f);
    for (int i = 0; i < melBins + 2; ++i)
        melPoints[static_cast<size_t>(i)] = melToHz(melMin + (melMax - melMin) * static_cast<float>(i) / static_cast<float>(melBins + 1));

    std::vector<int> binPoints(static_cast<size_t>(melBins + 2), 0);
    for (int i = 0; i < melBins + 2; ++i)
    {
        const float freq = melPoints[static_cast<size_t>(i)];
        const int bin = static_cast<int>(std::floor((fftSize + 1) * freq / sampleRate));
        binPoints[static_cast<size_t>(i)] = juce::jlimit(0, fftSize / 2, bin);
    }

    for (int frame = 0; frame < numFrames; ++frame)
    {
        const int startSample = frame * hopSize;
        std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);

        for (int i = 0; i < fftSize; ++i)
        {
            const int sampleIndex = startSample + i;
            if (sampleIndex >= numSamples)
                break;

            float sample = 0.0f;
            for (int channel = 0; channel < audio.getNumChannels(); ++channel)
                sample += audio.getSample(channel, sampleIndex);

            sample /= static_cast<float>(audio.getNumChannels());
            fftBuffer[static_cast<size_t>(i)] = sample;
        }

        window.multiplyWithWindowingTable(fftBuffer.data(), fftSize);
        fft.performRealOnlyForwardTransform(fftBuffer.data());

        for (int i = 0; i < fftSize / 2 + 1; ++i)
        {
            const float real = fftBuffer[static_cast<size_t>(2 * i)];
            const float imag = fftBuffer[static_cast<size_t>(2 * i + 1)];
            magnitude[static_cast<size_t>(i)] = std::sqrt(real * real + imag * imag);
        }

        for (int mel = 0; mel < melBins; ++mel)
        {
            const int left = binPoints[static_cast<size_t>(mel)];
            const int center = binPoints[static_cast<size_t>(mel + 1)];
            const int right = binPoints[static_cast<size_t>(mel + 2)];

            float sum = 0.0f;
            for (int i = left; i < center; ++i)
            {
                const float weight = (center == left) ? 0.0f : (static_cast<float>(i - left) / static_cast<float>(center - left));
                sum += magnitude[static_cast<size_t>(i)] * weight;
            }

            for (int i = center; i < right; ++i)
            {
                const float weight = (right == center) ? 0.0f : (static_cast<float>(right - i) / static_cast<float>(right - center));
                sum += magnitude[static_cast<size_t>(i)] * weight;
            }

            output.setSample(mel, frame, sum);
        }
    }

    return output;
}
