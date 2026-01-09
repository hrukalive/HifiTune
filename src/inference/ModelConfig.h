#pragma once

#include <optional>

#include <juce_core/juce_core.h>

class ModelConfig
{
public:
    static std::optional<ModelConfig> loadFromDirectory(const juce::File& directory);
    static std::optional<ModelConfig> loadFromModelFile(const juce::File& modelFile);
    static std::optional<ModelConfig> loadFromPath(const juce::File& path);

    const juce::File& getModelFile() const noexcept;
    const juce::File& getConfigFile() const noexcept;

    double getSamplingRate() const noexcept;
    int getHopSize() const noexcept;
    int getMelBins() const noexcept;
    int getWindowSize() const noexcept;
    const juce::StringArray& getExpectedInputs() const noexcept;

private:
    bool loadConfigFile(const juce::File& configFile);
    void applyConfigValue(const juce::String& key, const juce::var& value);
    void applyYamlLine(const juce::String& line, juce::String& currentKey);

    juce::File modelFile;
    juce::File configFile;

    double samplingRate { 44100.0 };
    int hopSize { 512 };
    int melBins { 80 };
    int windowSize { 2048 };
    juce::StringArray expectedInputs;
};
