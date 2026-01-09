#include "FcpePitchDetector.h"

#include "../analysis/MelSpectrogram.h"

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

struct ParsedOutput
{
    juce::Array<double> f0Curve;
    juce::Array<float> confidence;
};

ParsedOutput parseF0Output(const OnnxRuntimeEngine::TensorData& tensor)
{
    ParsedOutput parsed;
    if (! tensor.shape.empty() && tensor.shape.back() == 2)
    {
        const int frames = static_cast<int>(tensor.data.size() / 2);
        parsed.f0Curve.ensureStorageAllocated(frames);
        parsed.confidence.ensureStorageAllocated(frames);
        for (int i = 0; i < frames; ++i)
        {
            parsed.f0Curve.add(static_cast<double>(tensor.data[static_cast<size_t>(2 * i)]));
            parsed.confidence.add(tensor.data[static_cast<size_t>(2 * i + 1)]);
        }
    }
    else
    {
        parsed.f0Curve.ensureStorageAllocated(static_cast<int>(tensor.data.size()));
        for (float value : tensor.data)
            parsed.f0Curve.add(static_cast<double>(value));
    }

    return parsed;
}
}

FcpePitchDetector::FcpePitchDetector(ModelLoader& loader, const juce::File& modelPath)
    : model(loader.loadPitchDetectorFromPath(modelPath))
{
}

bool FcpePitchDetector::isValid() const
{
    return model != nullptr && model->engine != nullptr;
}

PitchDetectionResult FcpePitchDetector::detect(const juce::AudioBuffer<float>& audio, double sampleRate)
{
    PitchDetectionResult empty;
    if (! isValid())
        return empty;

    const auto targetRate = model->config.getSamplingRate();
    auto monoSamples = mixDownToMono(audio);
    monoSamples = resampleLinear(monoSamples, sampleRate, targetRate);
    normalize(monoSamples);

    juce::AudioBuffer<float> monoBuffer(1, static_cast<int>(monoSamples.size()));
    monoBuffer.copyFrom(0, 0, monoSamples.data(), static_cast<int>(monoSamples.size()));

    MelSpectrogram::Settings settings;
    settings.fftSize = model->config.getWindowSize();
    settings.hopSize = model->config.getHopSize();
    settings.melBins = model->config.getMelBins();
    settings.windowType = juce::dsp::WindowingFunction<float>::hann;

    MelSpectrogram spectrogram(settings);
    auto mel = spectrogram.render(monoBuffer, targetRate);

    const int melBins = mel.getNumChannels();
    const int frames = mel.getNumSamples();
    if (melBins == 0 || frames == 0)
        return empty;

    std::vector<float> features(static_cast<size_t>(melBins * frames), 0.0f);

    double sum = 0.0;
    double sumSquares = 0.0;
    const double count = static_cast<double>(melBins * frames);

    for (int melBin = 0; melBin < melBins; ++melBin)
    {
        for (int frame = 0; frame < frames; ++frame)
        {
            const float value = std::log1p(std::max(0.0f, mel.getSample(melBin, frame)));
            const size_t index = static_cast<size_t>(melBin * frames + frame);
            features[index] = value;
            sum += value;
            sumSquares += value * value;
        }
    }

    const double mean = sum / count;
    const double variance = (sumSquares / count) - (mean * mean);
    const double stdDev = variance > 0.0 ? std::sqrt(variance) : 1.0;

    for (auto& value : features)
        value = static_cast<float>((value - mean) / stdDev);

    OnnxRuntimeEngine::InferenceRequest request;
    auto expectedInputs = model->config.getExpectedInputs();
    const juce::String inputName = expectedInputs.isEmpty() ? juce::String("input") : expectedInputs[0];

    OnnxRuntimeEngine::TensorData inputTensor;
    inputTensor.name = inputName;
    inputTensor.shape = { 1, melBins, frames };
    inputTensor.data = std::move(features);
    request.inputs.push_back(std::move(inputTensor));

    auto inferenceResult = model->engine->run(request);
    if (inferenceResult.outputs.empty())
        return empty;

    auto parsed = parseF0Output(inferenceResult.outputs[0]);

    PitchDetectionResult detection;
    detection.hopSeconds = static_cast<double>(settings.hopSize) / targetRate;
    detection.f0Curve = parsed.f0Curve;
    detection.confidence = parsed.confidence;

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
