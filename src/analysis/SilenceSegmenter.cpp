#include "SilenceSegmenter.h"

#include <cmath>

SilenceSegmenter::SilenceSegmenter(Settings settingsIn)
    : settings(settingsIn)
{
}

const SilenceSegmenter::Settings& SilenceSegmenter::getSettings() const
{
    return settings;
}

void SilenceSegmenter::setSettings(Settings settingsIn)
{
    settings = settingsIn;
}

std::vector<SegmentState> SilenceSegmenter::detectSegments(const juce::AudioBuffer<float>& audio, double sampleRate) const
{
    std::vector<SegmentState> segments;
    const int numSamples = audio.getNumSamples();
    const int windowSize = settings.analysisWindow;
    const int hopSize = settings.analysisHop;

    if (numSamples == 0 || windowSize <= 0 || hopSize <= 0)
        return segments;

    const double silenceLinear = std::pow(10.0, settings.thresholdDb / 20.0);
    const int minSilenceSamples = static_cast<int>(std::round(settings.minSilenceSeconds * sampleRate));

    int segmentStartSample = -1;
    int silenceSamples = 0;

    for (int start = 0; start < numSamples; start += hopSize)
    {
        const int end = juce::jmin(start + windowSize, numSamples);
        double sumSquares = 0.0;
        int count = 0;

        for (int channel = 0; channel < audio.getNumChannels(); ++channel)
        {
            const float* data = audio.getReadPointer(channel, start);
            for (int i = start; i < end; ++i)
            {
                const float sample = data[i - start];
                sumSquares += sample * sample;
                ++count;
            }
        }

        const double rms = (count > 0) ? std::sqrt(sumSquares / static_cast<double>(count)) : 0.0;
        const bool isSilent = rms < silenceLinear;

        if (!isSilent)
        {
            if (segmentStartSample < 0)
                segmentStartSample = start;
            silenceSamples = 0;
        }
        else if (segmentStartSample >= 0)
        {
            silenceSamples += hopSize;
            if (silenceSamples >= minSilenceSamples)
            {
                const int segmentEndSample = juce::jlimit(segmentStartSample, numSamples, start);
                const double startSeconds = static_cast<double>(segmentStartSample) / sampleRate;
                const double lengthSeconds = static_cast<double>(segmentEndSample - segmentStartSample) / sampleRate;
                const double startBeat = startSeconds * settings.beatsPerSecond;
                const double lengthBeats = lengthSeconds * settings.beatsPerSecond;
                segments.push_back(SegmentState::create(startBeat, lengthBeats));
                segmentStartSample = -1;
            }
        }
    }

    if (segmentStartSample >= 0)
    {
        const double startSeconds = static_cast<double>(segmentStartSample) / sampleRate;
        const double lengthSeconds = static_cast<double>(numSamples - segmentStartSample) / sampleRate;
        const double startBeat = startSeconds * settings.beatsPerSecond;
        const double lengthBeats = lengthSeconds * settings.beatsPerSecond;
        segments.push_back(SegmentState::create(startBeat, lengthBeats));
    }

    return segments;
}
