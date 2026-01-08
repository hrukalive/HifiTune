#include "PluginEditor.h"

HifiTuneAudioProcessorEditor::HifiTuneAudioProcessorEditor(juce::AudioProcessor& processor)
    : juce::AudioProcessorEditor(processor)
{
    setSize(640, 400);
}

HifiTuneAudioProcessorEditor::~HifiTuneAudioProcessorEditor() = default;

void HifiTuneAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromString("#12131A"));
    g.setColour(juce::Colour::fromString("#F5F7FF"));
    g.setFont(18.0f);
    g.drawFittedText("HifiTune", getLocalBounds(), juce::Justification::centred, 1);
}

void HifiTuneAudioProcessorEditor::resized()
{
}
