#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

enum class ProgressState
{
    Idle,
    Active,
    Completed,
    Cancelled
};

class ProgressReporter
{
public:
    virtual ~ProgressReporter() = default;

    virtual void reportProgress(double progress, const juce::String& message) = 0;
    virtual void reportState(ProgressState state, const juce::String& message) = 0;
};

class GuiProgressReporter final : public ProgressReporter
{
public:
    using Callback = std::function<void(double progress, const juce::String& message, ProgressState state)>;

    explicit GuiProgressReporter(Callback callback);

    void reportProgress(double progress, const juce::String& message) override;
    void reportState(ProgressState state, const juce::String& message) override;

private:
    void dispatch(double progress, const juce::String& message, ProgressState state);

    Callback callback;
};
