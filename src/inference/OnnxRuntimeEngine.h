#pragma once

#include <functional>
#include <future>
#include <memory>
#include <vector>

#include <juce_core/juce_core.h>

class OnnxRuntimeEngine
{
public:
    enum class DeviceType
    {
        CPU,
        GPU
    };

    struct ThreadingOptions
    {
        int intraOpNumThreads { 0 };
        int interOpNumThreads { 0 };
    };

    struct TensorData
    {
        juce::String name;
        std::vector<int64_t> shape;
        std::vector<float> data;
    };

    struct InferenceRequest
    {
        std::vector<TensorData> inputs;
        juce::StringArray outputNames;
    };

    struct InferenceResult
    {
        std::vector<TensorData> outputs;
    };

    using ProgressCallback = std::function<void(double)>;

    static std::shared_ptr<OnnxRuntimeEngine> create(const juce::File& modelPath,
                                                     DeviceType deviceType,
                                                     ThreadingOptions threadingOptions);

    InferenceResult run(const InferenceRequest& request);
    std::future<InferenceResult> runAsync(const InferenceRequest& request,
                                          ProgressCallback progressCallback);

    const juce::File& getModelPath() const noexcept;
    DeviceType getDeviceType() const noexcept;

private:
    OnnxRuntimeEngine(const juce::File& modelPath,
                      DeviceType deviceType,
                      ThreadingOptions threadingOptions);

    struct Impl;
    std::shared_ptr<Impl> impl;
};
