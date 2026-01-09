#include "ModelConfig.h"

namespace
{
    juce::File findAdjacentConfigFile(const juce::File& modelFile)
    {
        const auto jsonConfig = modelFile.withFileExtension("json");
        if (jsonConfig.existsAsFile())
            return jsonConfig;

        const auto yamlConfig = modelFile.withFileExtension("yaml");
        if (yamlConfig.existsAsFile())
            return yamlConfig;

        const auto ymlConfig = modelFile.withFileExtension("yml");
        if (ymlConfig.existsAsFile())
            return ymlConfig;

        return {};
    }

    juce::File findDirectoryConfigFile(const juce::File& directory)
    {
        if (! directory.isDirectory())
            return {};

        auto jsonConfig = directory.getChildFile("config.json");
        if (jsonConfig.existsAsFile())
            return jsonConfig;

        auto yamlConfig = directory.getChildFile("config.yaml");
        if (yamlConfig.existsAsFile())
            return yamlConfig;

        auto ymlConfig = directory.getChildFile("config.yml");
        if (ymlConfig.existsAsFile())
            return ymlConfig;

        auto candidates = directory.findChildFiles(juce::File::findFiles, false, "*.json");
        if (! candidates.isEmpty())
            return candidates[0];

        candidates = directory.findChildFiles(juce::File::findFiles, false, "*.yaml");
        if (! candidates.isEmpty())
            return candidates[0];

        candidates = directory.findChildFiles(juce::File::findFiles, false, "*.yml");
        if (! candidates.isEmpty())
            return candidates[0];

        return {};
    }

    juce::File findConfigFileForModel(const juce::File& modelFile)
    {
        auto adjacent = findAdjacentConfigFile(modelFile);
        if (adjacent.existsAsFile())
            return adjacent;

        return findDirectoryConfigFile(modelFile.getParentDirectory());
    }

    juce::File findModelFile(const juce::File& directory)
    {
        auto modelFile = directory.getChildFile("model.onnx");
        if (modelFile.existsAsFile())
            return modelFile;

        auto candidates = directory.findChildFiles(juce::File::findFiles, false, "*.onnx");
        if (! candidates.isEmpty())
            return candidates[0];

        return {};
    }
}

std::optional<ModelConfig> ModelConfig::loadFromDirectory(const juce::File& directory)
{
    auto modelFile = findModelFile(directory);
    if (! modelFile.existsAsFile())
        return std::nullopt;

    return loadFromModelFile(modelFile);
}

std::optional<ModelConfig> ModelConfig::loadFromModelFile(const juce::File& modelFile)
{
    if (! modelFile.existsAsFile())
        return std::nullopt;

    ModelConfig config;
    config.modelFile = modelFile;
    config.configFile = findConfigFileForModel(modelFile);

    if (config.configFile.existsAsFile())
        config.loadConfigFile(config.configFile);

    return config;
}

std::optional<ModelConfig> ModelConfig::loadFromPath(const juce::File& path)
{
    if (path.isDirectory())
        return loadFromDirectory(path);

    return loadFromModelFile(path);
}

const juce::File& ModelConfig::getModelFile() const noexcept
{
    return modelFile;
}

const juce::File& ModelConfig::getConfigFile() const noexcept
{
    return configFile;
}

double ModelConfig::getSamplingRate() const noexcept
{
    return samplingRate;
}

int ModelConfig::getHopSize() const noexcept
{
    return hopSize;
}

int ModelConfig::getMelBins() const noexcept
{
    return melBins;
}

int ModelConfig::getWindowSize() const noexcept
{
    return windowSize;
}

const juce::StringArray& ModelConfig::getExpectedInputs() const noexcept
{
    return expectedInputs;
}

bool ModelConfig::loadConfigFile(const juce::File& configFileToLoad)
{
    const auto contents = configFileToLoad.loadFileAsString();
    if (contents.isEmpty())
        return false;

    if (configFileToLoad.hasFileExtension("json"))
    {
        auto parsed = juce::JSON::parse(contents);
        if (auto* object = parsed.getDynamicObject())
        {
            for (const auto& entry : object->getProperties())
                applyConfigValue(entry.name.toString(), entry.value);
            return true;
        }
        return false;
    }

    juce::String currentKey;
    auto lines = juce::StringArray::fromLines(contents);
    for (const auto& line : lines)
        applyYamlLine(line, currentKey);

    return true;
}

void ModelConfig::applyConfigValue(const juce::String& key, const juce::var& value)
{
    const auto normalizedKey = key.trim().toLowerCase();

    if (normalizedKey == "sampling_rate" || normalizedKey == "samplingrate")
    {
        samplingRate = static_cast<double>(value);
    }
    else if (normalizedKey == "hop_size" || normalizedKey == "hopsize")
    {
        hopSize = static_cast<int>(value);
    }
    else if (normalizedKey == "mel_bins" || normalizedKey == "melbins")
    {
        melBins = static_cast<int>(value);
    }
    else if (normalizedKey == "window_size" || normalizedKey == "windowsize")
    {
        windowSize = static_cast<int>(value);
    }
    else if (normalizedKey == "expected_inputs")
    {
        expectedInputs.clear();
        if (auto* array = value.getArray())
        {
            for (const auto& item : *array)
                expectedInputs.add(item.toString());
        }
        else if (value.isString())
        {
            expectedInputs.add(value.toString());
        }
    }
}

void ModelConfig::applyYamlLine(const juce::String& line, juce::String& currentKey)
{
    auto trimmed = line.trim();
    if (trimmed.isEmpty() || trimmed.startsWithChar('#'))
        return;

    if (trimmed.startsWithChar('-'))
    {
        if (currentKey == "expected_inputs")
            expectedInputs.add(trimmed.fromFirstOccurrenceOf("-", false, false).trim());
        return;
    }

    auto keyValue = juce::StringArray::fromTokens(trimmed, ":", "");
    if (keyValue.size() < 2)
        return;

    currentKey = keyValue[0].trim().toLowerCase();
    auto valueText = trimmed.fromFirstOccurrenceOf(":", false, false).trim();

    if (currentKey == "expected_inputs")
    {
        expectedInputs.clear();
        if (! valueText.isEmpty())
            expectedInputs.add(valueText);
        return;
    }

    if (valueText.containsOnly("0123456789."))
    {
        if (valueText.containsChar('.'))
            applyConfigValue(currentKey, valueText.getDoubleValue());
        else
            applyConfigValue(currentKey, valueText.getIntValue());
    }
    else
    {
        applyConfigValue(currentKey, valueText);
    }
}
