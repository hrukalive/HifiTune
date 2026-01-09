#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../commands/CommandManager.h"
#include "EditorMode.h"
#include "ToolbarComponent.h"
#include "SidebarComponent.h"
#include "NoteViewComponent.h"
#include "PitchViewComponent.h"
#include "QuickHelpOverlay.h"
#include "OverviewComponent.h"
#include "TimelineComponent.h"
#include "lookandfeel/EditorLookAndFeel.h"

class MainEditor final : public juce::Component,
                         public juce::ApplicationCommandTarget
{
public:
    MainEditor();
    ~MainEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void setMode(EditorMode mode);
    void setAutoFollow(bool enabled);
    void toggleQuickHelp();
    void updateQuickHelpContent();

    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    EditorLookAndFeel lookAndFeel;
    CommandManager commandManager;
    ToolbarComponent toolbar;
    SidebarComponent sidebar;
    PitchViewComponent pitchView;
    NoteViewComponent noteView;
    QuickHelpOverlay quickHelpOverlay;
    OverviewComponent overview;
    TimelineComponent timeline;
    juce::Label modeLabel;
    bool quickHelpVisible = false;
    bool autoFollowEnabled = true;

    EditorMode currentMode { EditorMode::PitchAmplitude };
};
