#include "ProgressReporter.h"

GuiProgressReporter::GuiProgressReporter(Callback callbackIn)
    : callback(std::move(callbackIn))
{
}

void GuiProgressReporter::reportProgress(double progress, const juce::String& message)
{
    dispatch(progress, message, ProgressState::Active);
}

void GuiProgressReporter::reportState(ProgressState state, const juce::String& message)
{
    dispatch(state == ProgressState::Active ? 0.0 : 1.0, message, state);
}

void GuiProgressReporter::dispatch(double progress, const juce::String& message, ProgressState state)
{
    if (!callback)
        return;

    auto callbackCopy = callback;
    juce::MessageManager::callAsync([callbackCopy, progress, message, state]()
    {
        callbackCopy(progress, message, state);
    });
}
