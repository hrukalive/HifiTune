#include "AnalysisTask.h"

AnalysisTask::AnalysisTask(juce::String nameIn)
    : name(std::move(nameIn))
{
}

const juce::String& AnalysisTask::getName() const
{
    return name;
}
