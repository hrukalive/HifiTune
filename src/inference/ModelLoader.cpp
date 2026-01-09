#include "ModelLoader.h"

std::mutex ModelLoader::cacheMutex;
std::map<juce::String, std::weak_ptr<OnnxRuntimeEngine>> ModelLoader::engineCache;

ModelLoader::ModelLoader(Options optionsToUse)
    : options(optionsToUse)
{
}

std::unique_ptr<PitchDetectorModel> ModelLoader::loadPitchDetector(const juce::File& directory)
{
    auto config = ModelConfig::loadFromDirectory(directory);
    if (! config.has_value())
        return {};

    auto model = std::make_unique<PitchDetectorModel>();
    model->config = *config;
    model->engine = getOrCreateEngine(config->getModelFile());
    return model;
}

std::unique_ptr<PitchDetectorModel> ModelLoader::loadPitchDetectorFromPath(const juce::File& path)
{
    auto config = ModelConfig::loadFromPath(path);
    if (! config.has_value())
        return {};

    auto model = std::make_unique<PitchDetectorModel>();
    model->config = *config;
    model->engine = getOrCreateEngine(config->getModelFile());
    return model;
}

std::unique_ptr<VocoderModel> ModelLoader::loadVocoder(const juce::File& directory)
{
    auto config = ModelConfig::loadFromDirectory(directory);
    if (! config.has_value())
        return {};

    auto model = std::make_unique<VocoderModel>();
    model->config = *config;
    model->engine = getOrCreateEngine(config->getModelFile());
    return model;
}

std::unique_ptr<SeparatorModel> ModelLoader::loadSeparator(const juce::File& directory)
{
    auto config = ModelConfig::loadFromDirectory(directory);
    if (! config.has_value())
        return {};

    auto model = std::make_unique<SeparatorModel>();
    model->config = *config;
    model->engine = getOrCreateEngine(config->getModelFile());
    return model;
}

std::shared_ptr<OnnxRuntimeEngine> ModelLoader::getOrCreateEngine(const juce::File& modelFile)
{
    const auto cacheKey = modelFile.getFullPathName();

    std::lock_guard lock(cacheMutex);

    if (auto existing = engineCache[cacheKey].lock())
        return existing;

    auto engine = OnnxRuntimeEngine::create(modelFile, options.deviceType, options.threadingOptions);
    engineCache[cacheKey] = engine;
    return engine;
}
