#pragma once

#include "PitchDetector.h"
#include "../inference/ModelLoader.h"

class RmvpePitchDetector : public PitchDetector
{
public:
    RmvpePitchDetector(ModelLoader& loader, const juce::File& modelPath);

    PitchDetectionResult detect(const juce::AudioBuffer<float>& audio, double sampleRate) override;

    bool isValid() const;

private:
    std::unique_ptr<PitchDetectorModel> model;
};
