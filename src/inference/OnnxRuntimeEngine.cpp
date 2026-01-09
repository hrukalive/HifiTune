#include "OnnxRuntimeEngine.h"

#include <algorithm>

#if defined(HIFITUNE_USE_ONNXRUNTIME)
#include <onnxruntime_cxx_api.h>
#endif

namespace
{
    std::vector<std::string> toStdStrings(const std::vector<OnnxRuntimeEngine::TensorData>& tensors)
    {
        std::vector<std::string> names;
        names.reserve(tensors.size());
        for (const auto& tensor : tensors)
            names.push_back(tensor.name.toStdString());
        return names;
    }

    std::vector<std::string> toStdStrings(const juce::StringArray& names)
    {
        std::vector<std::string> values;
        values.reserve(static_cast<size_t>(names.size()));
        for (const auto& name : names)
            values.push_back(name.toStdString());
        return values;
    }
}

struct OnnxRuntimeEngine::Impl
{
    juce::File modelPath;
    DeviceType deviceType { DeviceType::CPU };
    ThreadingOptions threadingOptions {};

#if defined(HIFITUNE_USE_ONNXRUNTIME)
    Ort::Env env { ORT_LOGGING_LEVEL_WARNING, "HiFiTune" };
    Ort::SessionOptions sessionOptions;
    Ort::Session session { nullptr };
    Ort::MemoryInfo memoryInfo { Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault) };
#endif
};

std::shared_ptr<OnnxRuntimeEngine> OnnxRuntimeEngine::create(const juce::File& modelPath,
                                                             DeviceType deviceType,
                                                             ThreadingOptions threadingOptions)
{
    return std::shared_ptr<OnnxRuntimeEngine>(new OnnxRuntimeEngine(modelPath, deviceType, threadingOptions));
}

OnnxRuntimeEngine::OnnxRuntimeEngine(const juce::File& modelPath,
                                     DeviceType deviceType,
                                     ThreadingOptions threadingOptions)
    : impl(std::make_shared<Impl>())
{
    impl->modelPath = modelPath;
    impl->deviceType = deviceType;
    impl->threadingOptions = threadingOptions;

#if defined(HIFITUNE_USE_ONNXRUNTIME)
    if (threadingOptions.intraOpNumThreads > 0)
        impl->sessionOptions.SetIntraOpNumThreads(threadingOptions.intraOpNumThreads);
    if (threadingOptions.interOpNumThreads > 0)
        impl->sessionOptions.SetInterOpNumThreads(threadingOptions.interOpNumThreads);

    const auto modelPathString = modelPath.getFullPathName().toStdString();
    impl->session = Ort::Session(impl->env, modelPathString.c_str(), impl->sessionOptions);
#else
    juce::ignoreUnused(modelPath, deviceType, threadingOptions);
#endif
}

OnnxRuntimeEngine::InferenceResult OnnxRuntimeEngine::run(const InferenceRequest& request)
{
    InferenceResult result;

#if defined(HIFITUNE_USE_ONNXRUNTIME)
    auto inputNames = toStdStrings(request.inputs);
    auto outputNames = toStdStrings(request.outputNames);

    std::vector<const char*> inputNamePointers;
    inputNamePointers.reserve(inputNames.size());
    for (const auto& name : inputNames)
        inputNamePointers.push_back(name.c_str());

    std::vector<const char*> outputNamePointers;
    outputNamePointers.reserve(outputNames.size());
    for (const auto& name : outputNames)
        outputNamePointers.push_back(name.c_str());

    std::vector<Ort::Value> inputValues;
    inputValues.reserve(request.inputs.size());

    for (const auto& tensor : request.inputs)
    {
        auto elementCount = static_cast<size_t>(tensor.data.size());
        auto* dataPointer = const_cast<float*>(tensor.data.data());
        inputValues.emplace_back(Ort::Value::CreateTensor<float>(impl->memoryInfo,
                                                                 dataPointer,
                                                                 elementCount,
                                                                 tensor.shape.data(),
                                                                 tensor.shape.size()));
    }

    auto outputValues = impl->session.Run(Ort::RunOptions { nullptr },
                                          inputNamePointers.data(),
                                          inputValues.data(),
                                          inputValues.size(),
                                          outputNamePointers.data(),
                                          outputNamePointers.size());

    result.outputs.reserve(outputValues.size());

    for (size_t index = 0; index < outputValues.size(); ++index)
    {
        auto& outputValue = outputValues[index];
        auto shapeInfo = outputValue.GetTensorTypeAndShapeInfo();
        auto shape = shapeInfo.GetShape();
        auto elementCount = shapeInfo.GetElementCount();

        TensorData outputTensor;
        outputTensor.name = outputNames.empty() ? juce::String("output_") + juce::String(static_cast<int>(index))
                                                : juce::String(outputNames[index]);
        outputTensor.shape = std::move(shape);
        outputTensor.data.resize(static_cast<size_t>(elementCount));

        auto* outputData = outputValue.GetTensorMutableData<float>();
        std::copy(outputData, outputData + elementCount, outputTensor.data.begin());

        result.outputs.push_back(std::move(outputTensor));
    }
#else
    juce::ignoreUnused(request);
#endif

    return result;
}

std::future<OnnxRuntimeEngine::InferenceResult> OnnxRuntimeEngine::runAsync(const InferenceRequest& request,
                                                                            ProgressCallback progressCallback)
{
    return std::async(std::launch::async, [this, request, progressCallback]() {
        if (progressCallback)
            progressCallback(0.0);

        auto result = run(request);

        if (progressCallback)
            progressCallback(1.0);

        return result;
    });
}

const juce::File& OnnxRuntimeEngine::getModelPath() const noexcept
{
    return impl->modelPath;
}

OnnxRuntimeEngine::DeviceType OnnxRuntimeEngine::getDeviceType() const noexcept
{
    return impl->deviceType;
}
