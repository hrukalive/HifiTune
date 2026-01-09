#pragma once

#include <vector>

#include <juce_data_structures/juce_data_structures.h>

#include "AudioBufferSource.h"
#include "../model/AnalysisConfigState.h"
#include "../model/NoteState.h"

class MidiNoteExtractor
{
public:
    virtual ~MidiNoteExtractor() = default;

    virtual juce::ValueTree analyze(const AudioBufferSource& audioSource,
                                    const AnalysisConfigState& config,
                                    juce::UndoManager* undoManager) = 0;

    static juce::ValueTree createResultTree();
    static bool isResultTree(const juce::ValueTree& tree);

    static void addNote(juce::ValueTree& tree, const NoteState& note, juce::UndoManager* undoManager);
    static std::vector<NoteState> getNotes(const juce::ValueTree& tree);

    static const juce::Identifier resultTypeId;
};
