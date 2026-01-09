#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "ToolbarComponent.h"
#include "SidebarComponent.h"
#include "NoteViewComponent.h"
#include "PitchViewComponent.h"
#include "OverviewComponent.h"
#include "TimelineComponent.h"
#include "lookandfeel/EditorLookAndFeel.h"

class MainEditor final : public juce::Component
{
public:
    MainEditor();
    ~MainEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void setMode(EditorMode mode);

    EditorLookAndFeel lookAndFeel;
    ToolbarComponent toolbar;
    SidebarComponent sidebar;
    PitchViewComponent pitchView;
    NoteViewComponent noteView;
    OverviewComponent overview;
    TimelineComponent timeline;
    juce::Label modeLabel;

    EditorMode currentMode { EditorMode::PitchAmplitude };
};
