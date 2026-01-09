#pragma once

#include <map>
#include <memory>
#include <mutex>

#include <juce_core/juce_core.h>

#include "ModelConfig.h"
#include "OnnxRuntimeEngine.h"

class PitchDetectorModel
{
public:
    ModelConfig config;
    std::shared_ptr<OnnxRuntimeEngine> engine;
};

class VocoderModel
{
public:
    ModelConfig config;
    std::shared_ptr<OnnxRuntimeEngine> engine;
};

class SeparatorModel
{
public:
    ModelConfig config;
    std::shared_ptr<OnnxRuntimeEngine> engine;
};

class ModelLoader
{
public:
    struct Options
    {
        OnnxRuntimeEngine::DeviceType deviceType { OnnxRuntimeEngine::DeviceType::CPU };
        OnnxRuntimeEngine::ThreadingOptions threadingOptions {};
    };

    explicit ModelLoader(Options options = {});

    std::unique_ptr<PitchDetectorModel> loadPitchDetector(const juce::File& directory);
    std::unique_ptr<PitchDetectorModel> loadPitchDetectorFromPath(const juce::File& path);
    std::unique_ptr<VocoderModel> loadVocoder(const juce::File& directory);
    std::unique_ptr<SeparatorModel> loadSeparator(const juce::File& directory);

private:
    std::shared_ptr<OnnxRuntimeEngine> getOrCreateEngine(const juce::File& modelFile);

    Options options;

    static std::mutex cacheMutex;
    static std::map<juce::String, std::weak_ptr<OnnxRuntimeEngine>> engineCache;
};
