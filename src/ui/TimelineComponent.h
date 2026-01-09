#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "lookandfeel/EditorLookAndFeel.h"

class TimelineComponent final : public juce::Component
{
public:
    TimelineComponent();
    ~TimelineComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    TimelineLookAndFeel lookAndFeel;
};
