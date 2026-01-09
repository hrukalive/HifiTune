#include "ToolbarComponent.h"

ToolbarComponent::ToolbarComponent()
{
    setLookAndFeel(&lookAndFeel);

    playButton.setToggleable(true);
    previewButton.setToggleable(true);
    playButton.setClickingTogglesState(true);
    previewButton.setClickingTogglesState(true);

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(previewButton);

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
    auto modeArea = bounds.removeFromLeft(bounds.getWidth() - 220);

    g.setColour(juce::Colour::fromString("#2D3247"));
    g.drawRoundedRectangle(transportArea.toFloat(), 6.0f, 1.0f);
    g.drawRoundedRectangle(modeArea.toFloat(), 6.0f, 1.0f);

    g.setColour(findColour(juce::Label::textColourId));
    g.setFont(12.0f);
    g.drawText("Transport", transportArea.removeFromTop(18), juce::Justification::centredLeft);
    g.drawText("Modes", modeArea.removeFromTop(18), juce::Justification::centredLeft);
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

    playButton.setBounds(transportArea.removeFromLeft(70).reduced(2));
    stopButton.setBounds(transportArea.removeFromLeft(70).reduced(2));
    previewButton.setBounds(transportArea.removeFromLeft(90).reduced(2));

    const int buttonHeight = bounds.getHeight();
    const int buttonWidth = juce::jmax(90, bounds.getWidth() / 6);

    for (int i = 0; i < 6; ++i)
    {
        modeButtons[i].setBounds(bounds.removeFromLeft(buttonWidth).reduced(3, 6));
        bounds.removeFromLeft(4);
    }
}
