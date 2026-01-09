#pragma once

#include <juce_core/juce_core.h>
#include <atomic>

class ProgressReporter;

class AnalysisTask
{
public:
    explicit AnalysisTask(juce::String name);
    virtual ~AnalysisTask() = default;

    const juce::String& getName() const;

    virtual void run(ProgressReporter& reporter, std::atomic<bool>& shouldCancel) = 0;

private:
    juce::String name;
};
