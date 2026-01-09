#pragma once

#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>

#include "../model/SegmentState.h"

class SilenceSegmenter
{
public:
    struct Settings
    {
        double thresholdDb{-60.0};
        double minSilenceSeconds{0.2};
        int analysisWindow{1024};
        int analysisHop{512};
        double beatsPerSecond{1.0};
    };

    explicit SilenceSegmenter(Settings settings);

    const Settings& getSettings() const;
    void setSettings(Settings settings);

    std::vector<SegmentState> detectSegments(const juce::AudioBuffer<float>& audio, double sampleRate) const;

private:
    Settings settings;
};
