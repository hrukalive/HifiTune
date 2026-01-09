#pragma once

#include "Vocoder.h"
#include "../inference/ModelLoader.h"

class NsfHifiGanVocoder final : public Vocoder
{
public:
    NsfHifiGanVocoder(ModelLoader& loader, const juce::File& modelPath);

    bool isValid() const override;

protected:
    juce::AudioBuffer<float> synthesize(const RenderInput& input,
                                        const RenderSettings& settings,
                                        int hopSize) override;

private:
    std::unique_ptr<VocoderModel> model;
};
