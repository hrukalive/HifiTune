#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "lookandfeel/EditorLookAndFeel.h"

class ToolbarComponent final : public juce::Component
{
public:
    ToolbarComponent();
    ~ToolbarComponent() override;

    void resized() override;

    void setMode(EditorMode mode);
    std::function<void(EditorMode)> onModeChange;

private:
    void handleModeClick(EditorMode mode);

    ToolbarLookAndFeel lookAndFeel;
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton previewButton { "Preview" };
    juce::TextButton modeButtons[6] {
        juce::TextButton("Pitch/Amplitude"),
        juce::TextButton("Timing"),
        juce::TextButton("Pitch Draw"),
        juce::TextButton("Temporary Note"),
        juce::TextButton("Note Assignment"),
        juce::TextButton("Harmony")
    };
    EditorMode currentMode { EditorMode::PitchAmplitude };
};
