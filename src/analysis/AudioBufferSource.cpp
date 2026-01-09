#include "AudioBufferSource.h"

#include <cmath>

AudioBufferSource::AudioBufferSource(const juce::AudioBuffer<float>& audioBuffer, double sampleRate)
    : buffer(audioBuffer), sampleRateHz(sampleRate)
{
}

int AudioBufferSource::getNumSamples() const
{
    return buffer.getNumSamples();
}

int AudioBufferSource::getNumChannels() const
{
    return buffer.getNumChannels();
}

double AudioBufferSource::getSampleRate() const
{
    return sampleRateHz;
}

juce::AudioBuffer<float> AudioBufferSource::getClip(int startSample, int numSamples) const
{
    const int safeStart = juce::jlimit(0, buffer.getNumSamples(), startSample);
    const int safeLength = juce::jlimit(0, buffer.getNumSamples() - safeStart, numSamples);

    juce::AudioBuffer<float> clip(buffer.getNumChannels(), safeLength);
    if (safeLength == 0)
        return clip;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        clip.copyFrom(channel, 0, buffer, channel, safeStart, safeLength);

    return clip;
}

juce::AudioBuffer<float> AudioBufferSource::getSegment(double startSeconds, double lengthSeconds) const
{
    const int startSample = static_cast<int>(std::round(startSeconds * sampleRateHz));
    const int numSamples = static_cast<int>(std::round(lengthSeconds * sampleRateHz));
    return getClip(startSample, numSamples);
}
