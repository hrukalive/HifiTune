#include "ModelRegistry.h"

ModelRegistry::ModelRegistry(ModelLoader& loaderToUse)
    : loader(loaderToUse)
{
}

void ModelRegistry::registerPitchDetector(const juce::String& identifier, PitchFactory factory)
{
    pitchFactories[identifier] = std::move(factory);
}

void ModelRegistry::registerVocoder(const juce::String& identifier, VocoderFactory factory)
{
    vocoderFactories[identifier] = std::move(factory);
}

void ModelRegistry::registerSeparator(const juce::String& identifier, SeparatorFactory factory)
{
    separatorFactories[identifier] = std::move(factory);
}

std::unique_ptr<PitchDetectorModel> ModelRegistry::createPitchDetector(const juce::String& identifier,
                                                                       const juce::File& directory) const
{
    if (auto it = pitchFactories.find(identifier); it != pitchFactories.end())
        return it->second(loader, directory);
    return {};
}

std::unique_ptr<VocoderModel> ModelRegistry::createVocoder(const juce::String& identifier,
                                                           const juce::File& directory) const
{
    if (auto it = vocoderFactories.find(identifier); it != vocoderFactories.end())
        return it->second(loader, directory);
    return {};
}

std::unique_ptr<SeparatorModel> ModelRegistry::createSeparator(const juce::String& identifier,
                                                               const juce::File& directory) const
{
    if (auto it = separatorFactories.find(identifier); it != separatorFactories.end())
        return it->second(loader, directory);
    return {};
}

juce::StringArray ModelRegistry::getPitchDetectorIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(pitchFactories.size()));
    for (const auto& entry : pitchFactories)
        ids.add(entry.first);
    return ids;
}

juce::StringArray ModelRegistry::getVocoderIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(vocoderFactories.size()));
    for (const auto& entry : vocoderFactories)
        ids.add(entry.first);
    return ids;
}

juce::StringArray ModelRegistry::getSeparatorIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(separatorFactories.size()));
    for (const auto& entry : separatorFactories)
        ids.add(entry.first);
    return ids;
}
