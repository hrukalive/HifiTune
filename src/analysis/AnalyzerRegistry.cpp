#include "AnalyzerRegistry.h"

void AnalyzerRegistry::registerPitchAnalyzer(const juce::String& identifier, PitchFactory factory)
{
    pitchFactories[identifier] = std::move(factory);
}

void AnalyzerRegistry::registerMidiNoteExtractor(const juce::String& identifier, MidiFactory factory)
{
    midiFactories[identifier] = std::move(factory);
}

void AnalyzerRegistry::registerHarmonicNoiseSeparator(const juce::String& identifier, SeparatorFactory factory)
{
    separatorFactories[identifier] = std::move(factory);
}

std::unique_ptr<PitchAnalyzer> AnalyzerRegistry::createPitchAnalyzer(const juce::String& identifier) const
{
    if (auto it = pitchFactories.find(identifier); it != pitchFactories.end())
        return it->second();
    return {};
}

std::unique_ptr<MidiNoteExtractor> AnalyzerRegistry::createMidiNoteExtractor(const juce::String& identifier) const
{
    if (auto it = midiFactories.find(identifier); it != midiFactories.end())
        return it->second();
    return {};
}

std::unique_ptr<HarmonicNoiseSeparator> AnalyzerRegistry::createHarmonicNoiseSeparator(const juce::String& identifier) const
{
    if (auto it = separatorFactories.find(identifier); it != separatorFactories.end())
        return it->second();
    return {};
}

juce::StringArray AnalyzerRegistry::getPitchAnalyzerIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(pitchFactories.size()));
    for (const auto& entry : pitchFactories)
        ids.add(entry.first);
    return ids;
}

juce::StringArray AnalyzerRegistry::getMidiNoteExtractorIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(midiFactories.size()));
    for (const auto& entry : midiFactories)
        ids.add(entry.first);
    return ids;
}

juce::StringArray AnalyzerRegistry::getHarmonicNoiseSeparatorIds() const
{
    juce::StringArray ids;
    ids.ensureStorageAllocated(static_cast<int>(separatorFactories.size()));
    for (const auto& entry : separatorFactories)
        ids.add(entry.first);
    return ids;
}
