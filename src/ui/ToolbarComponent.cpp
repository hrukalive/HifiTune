#include "ToolbarComponent.h"

ToolbarComponent::ToolbarComponent()
{
    setLookAndFeel(&lookAndFeel);

    playButton.setToggleable(true);
    previewButton.setToggleable(true);
    playButton.setClickingTogglesState(true);
    previewButton.setClickingTogglesState(true);
    autoFollowButton.setToggleable(true);
    autoFollowButton.setClickingTogglesState(false);

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(previewButton);
    addAndMakeVisible(autoFollowButton);
    addAndMakeVisible(helpButton);
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(analysisProgressBar);
    addAndMakeVisible(cancelButton);

    for (auto& button : modeButtons)
    {
        button.setClickingTogglesState(true);
        button.setRadioGroupId(1);
        addAndMakeVisible(button);
    }

    modeButtons[0].setToggleState(true, juce::NotificationType::dontSendNotification);

    modeButtons[0].onClick = [this] { handleModeClick(EditorMode::PitchAmplitude); };
    modeButtons[1].onClick = [this] { handleModeClick(EditorMode::Timing); };
    modeButtons[2].onClick = [this] { handleModeClick(EditorMode::PitchDraw); };
    modeButtons[3].onClick = [this] { handleModeClick(EditorMode::TemporaryNote); };
    modeButtons[4].onClick = [this] { handleModeClick(EditorMode::NoteAssignment); };
    modeButtons[5].onClick = [this] { handleModeClick(EditorMode::Harmony); };
    autoFollowButton.onClick = [this]
    {
        if (onAutoFollowReset)
            onAutoFollowReset();
    };
    helpButton.onClick = [this]
    {
        if (onQuickHelpToggle)
            onQuickHelpToggle();
    };
    stopButton.onClick = [this] { setPlaying(false); };
    cancelButton.onClick = [this]
    {
        if (onCancelAnalysis)
            onCancelAnalysis();
    };

    statusLabel.setText("Ready", juce::NotificationType::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    statusLabel.setColour(juce::Label::textColourId, findColour(juce::Label::textColourId));
    statusLabel.setFont(juce::Font(12.0f));
    statusLabel.setOpaque(false);
    cancelButton.setEnabled(false);
}

ToolbarComponent::~ToolbarComponent()
{
    setLookAndFeel(nullptr);
}

void ToolbarComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds().reduced(8);
    auto transportArea = bounds.removeFromLeft(240);
    auto actionsArea = bounds.removeFromRight(240);
    auto statusArea = bounds.removeFromRight(260);
    auto modeArea = bounds;

    g.setColour(juce::Colour::fromString("#2D3247"));
    g.drawRoundedRectangle(transportArea.toFloat(), 6.0f, 1.0f);
    g.drawRoundedRectangle(modeArea.toFloat(), 6.0f, 1.0f);
    g.drawRoundedRectangle(statusArea.toFloat(), 6.0f, 1.0f);
    g.drawRoundedRectangle(actionsArea.toFloat(), 6.0f, 1.0f);

    g.setColour(findColour(juce::Label::textColourId));
    g.setFont(12.0f);
    g.drawText("Transport", transportArea.removeFromTop(18), juce::Justification::centredLeft);
    g.drawText("Modes", modeArea.removeFromTop(18), juce::Justification::centredLeft);
    g.drawText("Status", statusArea.removeFromTop(18), juce::Justification::centredLeft);
    g.drawText("View", actionsArea.removeFromTop(18), juce::Justification::centredLeft);
}

void ToolbarComponent::setMode(EditorMode mode)
{
    currentMode = mode;

    const int index = [mode]
    {
        switch (mode)
        {
            case EditorMode::PitchAmplitude:
                return 0;
            case EditorMode::Timing:
                return 1;
            case EditorMode::PitchDraw:
                return 2;
            case EditorMode::TemporaryNote:
                return 3;
            case EditorMode::NoteAssignment:
                return 4;
            case EditorMode::Harmony:
                return 5;
        }

        return 0;
    }();

    for (int i = 0; i < 6; ++i)
    {
        modeButtons[i].setToggleState(i == index, juce::NotificationType::dontSendNotification);
    }
}

void ToolbarComponent::handleModeClick(EditorMode mode)
{
    setMode(mode);

    if (onModeChange)
        onModeChange(mode);
}

void ToolbarComponent::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    auto transportArea = bounds.removeFromLeft(240);
    auto actionsArea = bounds.removeFromRight(240);
    auto statusArea = bounds.removeFromRight(260);
    auto modeArea = bounds;

    playButton.setBounds(transportArea.removeFromLeft(70).reduced(2));
    stopButton.setBounds(transportArea.removeFromLeft(70).reduced(2));
    previewButton.setBounds(transportArea.removeFromLeft(90).reduced(2));

    const int buttonWidth = juce::jmax(90, modeArea.getWidth() / 6);

    for (int i = 0; i < 6; ++i)
    {
        modeButtons[i].setBounds(modeArea.removeFromLeft(buttonWidth).reduced(3, 6));
        modeArea.removeFromLeft(4);
    }

    autoFollowButton.setBounds(actionsArea.removeFromTop(actionsArea.getHeight() / 2).reduced(6, 6));
    helpButton.setBounds(actionsArea.reduced(6, 6));

    auto statusContent = statusArea.reduced(6, 6);
    statusLabel.setBounds(statusContent.removeFromTop(18));
    statusContent.removeFromTop(6);
    analysisProgressBar.setBounds(statusContent.removeFromTop(18));
    statusContent.removeFromTop(6);
    cancelButton.setBounds(statusContent.removeFromTop(22));
}

void ToolbarComponent::setAutoFollowEnabled(bool enabled)
{
    autoFollowButton.setToggleState(enabled, juce::NotificationType::dontSendNotification);
}

void ToolbarComponent::togglePlay()
{
    setPlaying(!playButton.getToggleState());
}

void ToolbarComponent::setPlaying(bool playing)
{
    playButton.setToggleState(playing, juce::NotificationType::sendNotification);
}

bool ToolbarComponent::isPlaying() const
{
    return playButton.getToggleState();
}

void ToolbarComponent::setAnalysisStatus(const juce::String& message, double progress, ProgressState state)
{
    analysisState = state;
    statusLabel.setText(message, juce::NotificationType::dontSendNotification);
    analysisProgress = progress;
    cancelButton.setEnabled(state == ProgressState::Active);
    analysisProgressBar.setVisible(state != ProgressState::Idle);
    repaint();
}
