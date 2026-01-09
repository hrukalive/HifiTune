#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "lookandfeel/EditorLookAndFeel.h"
#include "../tasks/ProgressReporter.h"

class ToolbarComponent final : public juce::Component
{
public:
    ToolbarComponent();
    ~ToolbarComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setMode(EditorMode mode);
    void setAutoFollowEnabled(bool enabled);
    void togglePlay();
    void setPlaying(bool playing);
    bool isPlaying() const;
    void setAnalysisStatus(const juce::String& message, double progress, ProgressState state);
    std::function<void(EditorMode)> onModeChange;
    std::function<void()> onAutoFollowReset;
    std::function<void()> onQuickHelpToggle;
    std::function<void()> onCancelAnalysis;

private:
    void handleModeClick(EditorMode mode);

    ToolbarLookAndFeel lookAndFeel;
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton previewButton { "Preview" };
    juce::TextButton autoFollowButton { "Auto-Follow" };
    juce::TextButton helpButton { "Quick Help" };
    juce::TextButton cancelButton { "Cancel" };
    juce::Label statusLabel;
    double analysisProgress { 0.0 };
    juce::ProgressBar analysisProgressBar { analysisProgress };
    juce::TextButton modeButtons[6] {
        juce::TextButton("Pitch/Amplitude"),
        juce::TextButton("Timing"),
        juce::TextButton("Pitch Draw"),
        juce::TextButton("Temporary Note"),
        juce::TextButton("Note Assignment"),
        juce::TextButton("Harmony")
    };
    EditorMode currentMode { EditorMode::PitchAmplitude };
    ProgressState analysisState { ProgressState::Idle };
};
