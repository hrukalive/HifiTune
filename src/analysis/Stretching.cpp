#include "Stretching.h"

#include <cmath>

namespace
{
float cubicInterpolate(float y0, float y1, float y2, float y3, float mu)
{
    const float a0 = y3 - y2 - y0 + y1;
    const float a1 = y0 - y1 - a0;
    const float a2 = y2 - y0;
    const float a3 = y1;
    return (a0 * mu * mu * mu + a1 * mu * mu + a2 * mu + a3);
}
}

juce::AudioBuffer<float> Stretching::stretchToHopSize(const juce::AudioBuffer<float>& input,
                                                     int sourceHopSize,
                                                     int targetHopSize,
                                                     InterpolationMode mode)
{
    const int numFrames = input.getNumSamples();
    if (numFrames == 0 || sourceHopSize <= 0 || targetHopSize <= 0)
        return {};

    const double ratio = static_cast<double>(sourceHopSize) / static_cast<double>(targetHopSize);
    const int outputFrames = juce::jmax(1, static_cast<int>(std::round(numFrames * ratio)));

    juce::AudioBuffer<float> output(input.getNumChannels(), outputFrames);

    for (int channel = 0; channel < input.getNumChannels(); ++channel)
    {
        for (int i = 0; i < outputFrames; ++i)
        {
            const double sourceIndex = static_cast<double>(i) / ratio;
            const int index = static_cast<int>(std::floor(sourceIndex));
            const double frac = sourceIndex - static_cast<double>(index);

            float value = 0.0f;
            if (mode == InterpolationMode::nearest)
            {
                const int nearestIndex = juce::jlimit(0, numFrames - 1, static_cast<int>(std::round(sourceIndex)));
                value = input.getSample(channel, nearestIndex);
            }
            else if (mode == InterpolationMode::linear)
            {
                const int nextIndex = juce::jlimit(0, numFrames - 1, index + 1);
                const float y0 = input.getSample(channel, juce::jlimit(0, numFrames - 1, index));
                const float y1 = input.getSample(channel, nextIndex);
                value = static_cast<float>((1.0 - frac) * y0 + frac * y1);
            }
            else
            {
                const int i0 = juce::jlimit(0, numFrames - 1, index - 1);
                const int i1 = juce::jlimit(0, numFrames - 1, index);
                const int i2 = juce::jlimit(0, numFrames - 1, index + 1);
                const int i3 = juce::jlimit(0, numFrames - 1, index + 2);
                value = cubicInterpolate(input.getSample(channel, i0),
                                         input.getSample(channel, i1),
                                         input.getSample(channel, i2),
                                         input.getSample(channel, i3),
                                         static_cast<float>(frac));
            }

            output.setSample(channel, i, value);
        }
    }

    return output;
}
