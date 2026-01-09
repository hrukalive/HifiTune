#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "lookandfeel/EditorLookAndFeel.h"

class SidebarComponent final : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SidebarLookAndFeel lookAndFeel;
    juce::GroupComponent settingsGroup { "settingsGroup", "Settings" };
    juce::GroupComponent scaleGroup { "scaleGroup", "Scale" };
    juce::GroupComponent midiRefGroup { "midiRefGroup", "MIDI Ref" };
    juce::GroupComponent timeGroup { "timeGroup", "Time" };

    juce::ComboBox settingsBox;
    juce::ComboBox scaleBox;
    juce::ComboBox midiRefBox;
    juce::ComboBox timeBox;
};
