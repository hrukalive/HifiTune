#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "lookandfeel/EditorLookAndFeel.h"

class PitchViewComponent final : public juce::Component
{
public:
    PitchViewComponent();
    ~PitchViewComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setMode(EditorMode mode);
    void setAmplitudeEnvelope(const std::vector<float>& envelope);
    void setPitchCurve(const std::vector<float>& curve);

private:
    PitchViewLookAndFeel lookAndFeel;
    std::vector<float> amplitudeEnvelope;
    std::vector<float> pitchCurve;
    EditorMode currentMode { EditorMode::PitchAmplitude };
};
