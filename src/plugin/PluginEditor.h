#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ui/MainEditor.h"

class HifiTuneAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit HifiTuneAudioProcessorEditor(juce::AudioProcessor& processor);
    ~HifiTuneAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    MainEditor mainEditor;
};
