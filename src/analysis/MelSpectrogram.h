#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class MelSpectrogram
{
public:
    struct Settings
    {
        int fftSize{2048};
        int hopSize{256};
        int melBins{128};
        juce::dsp::WindowingFunction<float>::WindowingMethod windowType{
            juce::dsp::WindowingFunction<float>::hann
        };
    };

    explicit MelSpectrogram(Settings settings);

    const Settings& getSettings() const;
    void setSettings(Settings settings);

    juce::AudioBuffer<float> render(const juce::AudioBuffer<float>& audio, double sampleRate) const;

private:
    Settings settings;
};
