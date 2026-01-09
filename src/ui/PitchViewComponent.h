#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "ViewportController.h"
#include "lookandfeel/EditorLookAndFeel.h"

class PitchViewComponent final : public juce::Component
{
public:
    PitchViewComponent();
    ~PitchViewComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void setMode(EditorMode mode);
    void setAmplitudeEnvelope(const std::vector<float>& envelope);
    void setPitchCurve(const std::vector<float>& curve);
    void setAutoFollow(bool enabled);

    std::function<void(bool)> onAutoFollowChanged;

private:
    void updateContentRanges();
    void updateAutoFollowState(bool enabled, bool refit);

    PitchViewLookAndFeel lookAndFeel;
    std::vector<float> amplitudeEnvelope;
    std::vector<float> pitchCurve;
    ViewportController viewport;
    bool autoFollowEnabled = true;
    EditorMode currentMode { EditorMode::PitchAmplitude };
};
