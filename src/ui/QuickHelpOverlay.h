#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class QuickHelpOverlay final : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    void setShortcuts(const juce::StringArray& shortcuts);

    std::function<void()> onDismiss;

private:
    juce::StringArray shortcutLines;
    juce::Rectangle<int> contentArea;
};
