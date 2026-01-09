#include "PluginEditor.h"

HifiTuneAudioProcessorEditor::HifiTuneAudioProcessorEditor(juce::AudioProcessor& processor)
    : juce::AudioProcessorEditor(processor)
{
    setSize(1280, 720);
    addAndMakeVisible(mainEditor);
    mainEditor.grabKeyboardFocus();
}

HifiTuneAudioProcessorEditor::~HifiTuneAudioProcessorEditor() = default;

void HifiTuneAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromString("#12131A"));
}

void HifiTuneAudioProcessorEditor::resized()
{
    mainEditor.setBounds(getLocalBounds());
}
