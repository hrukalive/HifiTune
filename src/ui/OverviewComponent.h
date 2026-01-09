#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "lookandfeel/EditorLookAndFeel.h"

class OverviewComponent final : public juce::Component
{
public:
    OverviewComponent();
    ~OverviewComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setViewRange(double start, double end);

private:
    OverviewLookAndFeel lookAndFeel;
    double viewStart = 0.0;
    double viewEnd = 4.0;
};
