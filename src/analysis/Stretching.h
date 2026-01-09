#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class Stretching
{
public:
    enum class InterpolationMode
    {
        nearest,
        linear,
        cubic
    };

    static juce::AudioBuffer<float> stretchToHopSize(const juce::AudioBuffer<float>& input,
                                                     int sourceHopSize,
                                                     int targetHopSize,
                                                     InterpolationMode mode);
};
