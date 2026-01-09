#include "MainEditor.h"
#include "../commands/CommandIDs.h"

MainEditor::MainEditor()
{
    setLookAndFeel(&lookAndFeel);
    setWantsKeyboardFocus(true);

    commandManager.registerTarget(this);
    commandManager.getKeyMappings().resetToDefaultMappings();
    addKeyListener(&commandManager.getKeyMappings());

    toolbar.onModeChange = [this](EditorMode mode) { setMode(mode); };
    toolbar.onAutoFollowReset = [this] { setAutoFollow(true); };
    toolbar.onQuickHelpToggle = [this] { toggleQuickHelp(); };

    pitchView.onAutoFollowChanged = [this](bool enabled) { setAutoFollow(enabled); };
    noteView.onAutoFollowChanged = [this](bool enabled) { setAutoFollow(enabled); };

    modeLabel.setText("Mode: " + toString(currentMode), juce::NotificationType::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centredRight);

    addAndMakeVisible(toolbar);
    addAndMakeVisible(sidebar);
    addAndMakeVisible(pitchView);
    addAndMakeVisible(noteView);
    addAndMakeVisible(overview);
    addAndMakeVisible(timeline);
    addAndMakeVisible(modeLabel);
    addAndMakeVisible(quickHelpOverlay);
    quickHelpOverlay.setInterceptsMouseClicks(true, true);
    quickHelpOverlay.setVisible(false);
    quickHelpOverlay.onDismiss = [this]
    {
        quickHelpVisible = false;
        quickHelpOverlay.setVisible(false);
    };

    toolbar.setAutoFollowEnabled(autoFollowEnabled);
    updateQuickHelpContent();
}

MainEditor::~MainEditor()
{
    setLookAndFeel(nullptr);
}

void MainEditor::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
}

void MainEditor::resized()
{
    auto bounds = getLocalBounds();
    auto toolbarArea = bounds.removeFromTop(56);
    toolbar.setBounds(toolbarArea.removeFromLeft(toolbarArea.getWidth() - 160));
    modeLabel.setBounds(toolbarArea.reduced(8, 12));

    auto timelineArea = bounds.removeFromBottom(32);
    timeline.setBounds(timelineArea);

    auto overviewArea = bounds.removeFromBottom(72);
    overview.setBounds(overviewArea);

    auto sidebarArea = bounds.removeFromLeft(240);
    sidebar.setBounds(sidebarArea);

    auto pitchArea = bounds.removeFromTop(juce::roundToInt(bounds.getHeight() * 0.55));
    pitchView.setBounds(pitchArea.reduced(4));
    noteView.setBounds(bounds.reduced(4));
    quickHelpOverlay.setBounds(getLocalBounds());
}

void MainEditor::setMode(EditorMode mode)
{
    currentMode = mode;
    toolbar.setMode(mode);
    pitchView.setMode(mode);
    noteView.setMode(mode);
    modeLabel.setText("Mode: " + toString(currentMode), juce::NotificationType::dontSendNotification);
}

void MainEditor::setAutoFollow(bool enabled)
{
    if (autoFollowEnabled == enabled)
        return;

    autoFollowEnabled = enabled;
    pitchView.setAutoFollow(enabled);
    noteView.setAutoFollow(enabled);
    toolbar.setAutoFollowEnabled(enabled);
}

void MainEditor::toggleQuickHelp()
{
    quickHelpVisible = !quickHelpVisible;
    quickHelpOverlay.setVisible(quickHelpVisible);
    if (quickHelpVisible)
        updateQuickHelpContent();
}

void MainEditor::updateQuickHelpContent()
{
    juce::StringArray lines;
    const juce::CommandID commands[] = {
        CommandIDs::playToggle,
        CommandIDs::undo,
        CommandIDs::redo,
        CommandIDs::savePreset,
        CommandIDs::autoFollowReset,
        CommandIDs::toggleQuickHelp
    };

    for (const auto commandID : commands)
    {
        juce::ApplicationCommandInfo info(commandID);
        getCommandInfo(commandID, info);

        auto keys = commandManager.getKeyMappings().getKeyPressesAssignedToCommand(commandID);
        if (keys.isEmpty())
            continue;

        juce::StringArray keyDescriptions;
        for (const auto& key : keys)
            keyDescriptions.add(key.getTextDescription());

        lines.add(keyDescriptions.joinIntoString(" / ") + ": " + info.shortName);
    }

    quickHelpOverlay.setShortcuts(lines);
}

juce::ApplicationCommandTarget* MainEditor::getNextCommandTarget()
{
    return nullptr;
}

void MainEditor::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    commands.addArray({
        CommandIDs::playToggle,
        CommandIDs::undo,
        CommandIDs::redo,
        CommandIDs::savePreset,
        CommandIDs::autoFollowReset,
        CommandIDs::toggleQuickHelp
    });
}

void MainEditor::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case CommandIDs::playToggle:
            result.setInfo("Play/Stop", "Toggle playback", "Transport", 0);
            result.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
            break;
        case CommandIDs::undo:
            result.setInfo("Undo", "Undo last action", "Edit", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
            result.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier);
            break;
        case CommandIDs::redo:
            result.setInfo("Redo", "Redo last action", "Edit", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            result.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::savePreset:
            result.setInfo("Save preset", "Save a preset", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
            result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
            break;
        case CommandIDs::autoFollowReset:
            result.setInfo("Reset auto-follow", "Re-enable auto-follow view", "View", 0);
            result.addDefaultKeypress('0', juce::ModifierKeys::commandModifier);
            result.addDefaultKeypress('0', juce::ModifierKeys::ctrlModifier);
            break;
        case CommandIDs::toggleQuickHelp:
            result.setInfo("Quick Help", "Toggle shortcut overlay", "Help", 0);
            result.addDefaultKeypress('?', 0);
            result.addDefaultKeypress(juce::KeyPress::F1Key, 0);
            break;
        default:
            break;
    }
}

bool MainEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::playToggle:
            toolbar.togglePlay();
            return true;
        case CommandIDs::undo:
            juce::Logger::writeToLog("Undo triggered.");
            return true;
        case CommandIDs::redo:
            juce::Logger::writeToLog("Redo triggered.");
            return true;
        case CommandIDs::savePreset:
            juce::Logger::writeToLog("Save preset triggered.");
            return true;
        case CommandIDs::autoFollowReset:
            setAutoFollow(true);
            return true;
        case CommandIDs::toggleQuickHelp:
            toggleQuickHelp();
            return true;
        default:
            return false;
    }
}
