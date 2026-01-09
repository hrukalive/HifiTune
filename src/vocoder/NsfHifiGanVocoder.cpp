#include "NsfHifiGanVocoder.h"

#include "../inference/OnnxRuntimeEngine.h"

#include <algorithm>
#include <cmath>

namespace
{
std::vector<float> resampleToLength(const std::vector<float>& samples, int targetLength)
{
    if (samples.empty() || targetLength <= 0)
        return {};

    if (static_cast<int>(samples.size()) == targetLength)
        return samples;

    std::vector<float> resampled(static_cast<size_t>(targetLength), 0.0f);
    const double ratio = static_cast<double>(samples.size() - 1) / static_cast<double>(targetLength - 1);

    for (int i = 0; i < targetLength; ++i)
    {
        const double sourceIndex = ratio * static_cast<double>(i);
        const int index = static_cast<int>(std::floor(sourceIndex));
        const double frac = sourceIndex - static_cast<double>(index);

        const int index0 = juce::jlimit(0, static_cast<int>(samples.size()) - 1, index);
        const int index1 = juce::jlimit(0, static_cast<int>(samples.size()) - 1, index + 1);
        const float value0 = samples[static_cast<size_t>(index0)];
        const float value1 = samples[static_cast<size_t>(index1)];
        resampled[static_cast<size_t>(i)] = static_cast<float>((1.0 - frac) * value0 + frac * value1);
    }

    return resampled;
}

std::vector<float> flattenMel(const juce::AudioBuffer<float>& melSpectrogram, int frames)
{
    const int melBins = melSpectrogram.getNumChannels();
    std::vector<float> data(static_cast<size_t>(melBins * frames), 0.0f);

    for (int mel = 0; mel < melBins; ++mel)
    {
        for (int frame = 0; frame < frames; ++frame)
        {
            const float value = melSpectrogram.getSample(mel, frame);
            data[static_cast<size_t>(mel * frames + frame)] = value;
        }
    }

    return data;
}

std::vector<float> buildPitchCurve(const juce::Array<double>& pitchCurve, int frames)
{
    if (frames <= 0)
        return {};

    if (pitchCurve.isEmpty())
        return std::vector<float>(static_cast<size_t>(frames), 0.0f);

    std::vector<float> curve;
    curve.reserve(static_cast<size_t>(pitchCurve.size()));
    for (const auto value : pitchCurve)
        curve.push_back(static_cast<float>(value));

    return resampleToLength(curve, frames);
}

juce::AudioBuffer<float> toAudioBuffer(const std::vector<float>& samples, int channels)
{
    const int numSamples = static_cast<int>(samples.size());
    juce::AudioBuffer<float> buffer(std::max(1, channels), numSamples);
    buffer.clear();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* dest = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i)
            dest[i] = samples[static_cast<size_t>(i)];
    }

    return buffer;
}
}

NsfHifiGanVocoder::NsfHifiGanVocoder(ModelLoader& loader, const juce::File& modelPath)
    : model(loader.loadVocoder(modelPath))
{
}

bool NsfHifiGanVocoder::isValid() const
{
    return model != nullptr && model->engine != nullptr;
}

juce::AudioBuffer<float> NsfHifiGanVocoder::synthesize(const RenderInput& input,
                                                       const RenderSettings&,
                                                       int hopSize)
{
    if (! isValid())
        return {};

    const int frames = input.melSpectrogram.getNumSamples();
    const int melBins = input.melSpectrogram.getNumChannels();
    if (frames <= 0 || melBins <= 0)
        return {};

    const auto pitchCurve = buildPitchCurve(input.pitchCurve, frames);
    auto melData = flattenMel(input.melSpectrogram, frames);

    OnnxRuntimeEngine::InferenceRequest request;
    auto expectedInputs = model->config.getExpectedInputs();
    juce::String melName = "mel";
    juce::String pitchName = "f0";

    if (expectedInputs.size() >= 2)
    {
        for (const auto& inputName : expectedInputs)
        {
            const auto lower = inputName.toLowerCase();
            if (lower.contains("mel"))
                melName = inputName;
            else if (lower.contains("f0") || lower.contains("pitch"))
                pitchName = inputName;
        }
    }
    else if (expectedInputs.size() == 1)
    {
        melName = expectedInputs[0];
    }

    OnnxRuntimeEngine::TensorData melTensor;
    melTensor.name = melName;
    melTensor.shape = { 1, melBins, frames };
    melTensor.data = std::move(melData);
    request.inputs.push_back(std::move(melTensor));

    OnnxRuntimeEngine::TensorData pitchTensor;
    pitchTensor.name = pitchName;
    pitchTensor.shape = { 1, 1, frames };
    pitchTensor.data = pitchCurve;
    request.inputs.push_back(std::move(pitchTensor));

    auto result = model->engine->run(request);
    if (result.outputs.empty())
        return {};

    const auto& audioTensor = result.outputs.front();
    if (audioTensor.data.empty())
        return {};

    std::vector<float> audioData = audioTensor.data;
    const int expectedSamples = frames * std::max(1, hopSize);
    const int targetSamples = input.rawAudio.getNumSamples() > 0 ? input.rawAudio.getNumSamples() : expectedSamples;
    if (targetSamples > 0 && static_cast<int>(audioData.size()) != targetSamples)
        audioData = resampleToLength(audioData, targetSamples);

    return toAudioBuffer(audioData, std::max(1, input.rawAudio.getNumChannels()));
}
