#pragma once

#include <functional>
#include <map>
#include <memory>

#include <juce_core/juce_core.h>

#include "HarmonicNoiseSeparator.h"
#include "MidiNoteExtractor.h"
#include "PitchAnalyzer.h"

class AnalyzerRegistry
{
public:
    using PitchFactory = std::function<std::unique_ptr<PitchAnalyzer>()>;
    using MidiFactory = std::function<std::unique_ptr<MidiNoteExtractor>()>;
    using SeparatorFactory = std::function<std::unique_ptr<HarmonicNoiseSeparator>()>;

    void registerPitchAnalyzer(const juce::String& identifier, PitchFactory factory);
    void registerMidiNoteExtractor(const juce::String& identifier, MidiFactory factory);
    void registerHarmonicNoiseSeparator(const juce::String& identifier, SeparatorFactory factory);

    std::unique_ptr<PitchAnalyzer> createPitchAnalyzer(const juce::String& identifier) const;
    std::unique_ptr<MidiNoteExtractor> createMidiNoteExtractor(const juce::String& identifier) const;
    std::unique_ptr<HarmonicNoiseSeparator> createHarmonicNoiseSeparator(const juce::String& identifier) const;

    juce::StringArray getPitchAnalyzerIds() const;
    juce::StringArray getMidiNoteExtractorIds() const;
    juce::StringArray getHarmonicNoiseSeparatorIds() const;

private:
    std::map<juce::String, PitchFactory> pitchFactories;
    std::map<juce::String, MidiFactory> midiFactories;
    std::map<juce::String, SeparatorFactory> separatorFactories;
};
