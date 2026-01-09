#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class AudioBufferSource
{
public:
    AudioBufferSource(const juce::AudioBuffer<float>& audioBuffer, double sampleRate);

    int getNumSamples() const;
    int getNumChannels() const;
    double getSampleRate() const;

    juce::AudioBuffer<float> getClip(int startSample, int numSamples) const;
    juce::AudioBuffer<float> getSegment(double startSeconds, double lengthSeconds) const;

private:
    const juce::AudioBuffer<float>& buffer;
    double sampleRateHz{0.0};
};
