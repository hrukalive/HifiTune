#pragma once

#include <functional>
#include <map>
#include <memory>

#include <juce_core/juce_core.h>

#include "ModelLoader.h"

class ModelRegistry
{
public:
    using PitchFactory = std::function<std::unique_ptr<PitchDetectorModel>(ModelLoader&, const juce::File&)>;
    using VocoderFactory = std::function<std::unique_ptr<VocoderModel>(ModelLoader&, const juce::File&)>;
    using SeparatorFactory = std::function<std::unique_ptr<SeparatorModel>(ModelLoader&, const juce::File&)>;

    explicit ModelRegistry(ModelLoader& loaderToUse);

    void registerPitchDetector(const juce::String& identifier, PitchFactory factory);
    void registerVocoder(const juce::String& identifier, VocoderFactory factory);
    void registerSeparator(const juce::String& identifier, SeparatorFactory factory);

    std::unique_ptr<PitchDetectorModel> createPitchDetector(const juce::String& identifier,
                                                            const juce::File& directory) const;
    std::unique_ptr<VocoderModel> createVocoder(const juce::String& identifier,
                                                const juce::File& directory) const;
    std::unique_ptr<SeparatorModel> createSeparator(const juce::String& identifier,
                                                    const juce::File& directory) const;

    juce::StringArray getPitchDetectorIds() const;
    juce::StringArray getVocoderIds() const;
    juce::StringArray getSeparatorIds() const;

private:
    ModelLoader& loader;
    std::map<juce::String, PitchFactory> pitchFactories;
    std::map<juce::String, VocoderFactory> vocoderFactories;
    std::map<juce::String, SeparatorFactory> separatorFactories;
};
