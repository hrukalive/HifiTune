#include "MainEditor.h"

MainEditor::MainEditor()
{
    setLookAndFeel(&lookAndFeel);

    toolbar.onModeChange = [this](EditorMode mode) { setMode(mode); };

    modeLabel.setText("Mode: " + toString(currentMode), juce::NotificationType::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centredRight);

    addAndMakeVisible(toolbar);
    addAndMakeVisible(sidebar);
    addAndMakeVisible(pitchView);
    addAndMakeVisible(noteView);
    addAndMakeVisible(overview);
    addAndMakeVisible(timeline);
    addAndMakeVisible(modeLabel);
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

    auto pitchArea = bounds.removeFromTop(bounds.getHeight() * 0.55);
    pitchView.setBounds(pitchArea.reduced(4));
    noteView.setBounds(bounds.reduced(4));
}

void MainEditor::setMode(EditorMode mode)
{
    currentMode = mode;
    toolbar.setMode(mode);
    pitchView.setMode(mode);
    noteView.setMode(mode);
    modeLabel.setText("Mode: " + toString(currentMode), juce::NotificationType::dontSendNotification);
}
