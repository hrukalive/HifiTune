#include "RmvpePitchDetector.h"

#include <algorithm>
#include <cmath>

namespace
{
std::vector<float> mixDownToMono(const juce::AudioBuffer<float>& audio)
{
    const int numSamples = audio.getNumSamples();
    const int numChannels = juce::jmax(1, audio.getNumChannels());
    std::vector<float> mono(static_cast<size_t>(numSamples), 0.0f);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float sum = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
            sum += audio.getSample(channel, sample);
        mono[static_cast<size_t>(sample)] = sum / static_cast<float>(numChannels);
    }

    return mono;
}

std::vector<float> resampleLinear(const std::vector<float>& samples,
                                  double sourceRate,
                                  double targetRate)
{
    if (samples.empty() || sourceRate <= 0.0 || targetRate <= 0.0 || sourceRate == targetRate)
        return samples;

    const double ratio = targetRate / sourceRate;
    const int outputSamples = juce::jmax(1, static_cast<int>(std::round(samples.size() * ratio)));
    std::vector<float> resampled(static_cast<size_t>(outputSamples), 0.0f);

    for (int i = 0; i < outputSamples; ++i)
    {
        const double sourceIndex = static_cast<double>(i) / ratio;
        const int index = static_cast<int>(std::floor(sourceIndex));
        const double frac = sourceIndex - static_cast<double>(index);

        const int index0 = juce::jlimit(0, static_cast<int>(samples.size()) - 1, index);
        const int index1 = juce::jlimit(0, static_cast<int>(samples.size()) - 1, index + 1);
        const float sample0 = samples[static_cast<size_t>(index0)];
        const float sample1 = samples[static_cast<size_t>(index1)];
        resampled[static_cast<size_t>(i)] = static_cast<float>((1.0 - frac) * sample0 + frac * sample1);
    }

    return resampled;
}

void normalize(std::vector<float>& samples)
{
    float peak = 0.0f;
    for (float value : samples)
        peak = std::max(peak, std::abs(value));

    if (peak <= 0.0f)
        return;

    const float scale = 0.99f / peak;
    for (auto& value : samples)
        value *= scale;
}

struct WindowedFrames
{
    std::vector<float> data;
    int frames { 0 };
    int windowSize { 0 };
};

WindowedFrames createWindowedFrames(const std::vector<float>& samples, int windowSize, int hopSize)
{
    WindowedFrames result;
    if (samples.empty() || windowSize <= 0 || hopSize <= 0)
        return result;

    const int numSamples = static_cast<int>(samples.size());
    const int numFrames = juce::jmax(1, 1 + (numSamples - 1) / hopSize);

    result.frames = numFrames;
    result.windowSize = windowSize;
    result.data.resize(static_cast<size_t>(numFrames * windowSize), 0.0f);

    std::vector<float> window(static_cast<size_t>(windowSize), 1.0f);
    for (int i = 0; i < windowSize; ++i)
        window[static_cast<size_t>(i)] = 0.5f - 0.5f * std::cos(2.0 * juce::MathConstants<double>::pi * i / windowSize);

    for (int frame = 0; frame < numFrames; ++frame)
    {
        const int startSample = frame * hopSize;
        for (int i = 0; i < windowSize; ++i)
        {
            const int sampleIndex = startSample + i;
            if (sampleIndex >= numSamples)
                break;
            const float sample = samples[static_cast<size_t>(sampleIndex)];
            result.data[static_cast<size_t>(frame * windowSize + i)] = sample * window[static_cast<size_t>(i)];
        }
    }

    return result;
}

PitchDetectionResult parseResult(const OnnxRuntimeEngine::InferenceResult& result,
                                 double hopSeconds)
{
    PitchDetectionResult detection;
    detection.hopSeconds = hopSeconds;

    if (result.outputs.empty())
        return detection;

    const auto& f0Tensor = result.outputs[0];
    detection.f0Curve.ensureStorageAllocated(static_cast<int>(f0Tensor.data.size()));
    for (float value : f0Tensor.data)
        detection.f0Curve.add(static_cast<double>(value));

    if (result.outputs.size() > 1)
    {
        const auto& confidenceTensor = result.outputs[1];
        detection.confidence.ensureStorageAllocated(static_cast<int>(confidenceTensor.data.size()));
        for (float value : confidenceTensor.data)
            detection.confidence.add(value);
    }

    return detection;
}
}

RmvpePitchDetector::RmvpePitchDetector(ModelLoader& loader, const juce::File& modelPath)
    : model(loader.loadPitchDetectorFromPath(modelPath))
{
}

bool RmvpePitchDetector::isValid() const
{
    return model != nullptr && model->engine != nullptr;
}

PitchDetectionResult RmvpePitchDetector::detect(const juce::AudioBuffer<float>& audio, double sampleRate)
{
    PitchDetectionResult empty;
    if (! isValid())
        return empty;

    const auto targetRate = model->config.getSamplingRate();
    auto monoSamples = mixDownToMono(audio);
    monoSamples = resampleLinear(monoSamples, sampleRate, targetRate);
    normalize(monoSamples);

    int windowSize = model->config.getWindowSize();
    int hopSize = model->config.getHopSize();
    if (windowSize <= 0)
        windowSize = 1024;
    if (hopSize <= 0)
        hopSize = windowSize / 4;

    auto frames = createWindowedFrames(monoSamples, windowSize, hopSize);
    if (frames.frames == 0)
        return empty;

    OnnxRuntimeEngine::InferenceRequest request;
    auto expectedInputs = model->config.getExpectedInputs();
    const juce::String inputName = expectedInputs.isEmpty() ? juce::String("input") : expectedInputs[0];

    OnnxRuntimeEngine::TensorData inputTensor;
    inputTensor.name = inputName;
    inputTensor.shape = { 1, frames.frames, frames.windowSize };
    inputTensor.data = std::move(frames.data);
    request.inputs.push_back(std::move(inputTensor));

    auto inferenceResult = model->engine->run(request);
    auto detection = parseResult(inferenceResult, static_cast<double>(hopSize) / targetRate);

    const int numPoints = detection.f0Curve.size();
    detection.confidence.ensureStorageAllocated(numPoints);
    if (detection.confidence.size() < numPoints)
    {
        for (int i = detection.confidence.size(); i < numPoints; ++i)
            detection.confidence.add(1.0f);
    }

    for (int i = 0; i < numPoints; ++i)
    {
        const float baseConfidence = detection.confidence[i];
        detection.confidence.set(i, PitchDetector::computeTuningConfidence(detection.f0Curve[i], baseConfidence));
    }

    return detection;
}
