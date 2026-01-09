#include "MidiNoteExtractor.h"

const juce::Identifier MidiNoteExtractor::resultTypeId{"MidiNoteAnalysisResult"};

juce::ValueTree MidiNoteExtractor::createResultTree()
{
    return juce::ValueTree(resultTypeId);
}

bool MidiNoteExtractor::isResultTree(const juce::ValueTree& tree)
{
    return tree.hasType(resultTypeId);
}

void MidiNoteExtractor::addNote(juce::ValueTree& tree, const NoteState& note, juce::UndoManager* undoManager)
{
    if (!tree.hasType(resultTypeId) || !note.isValid())
        return;

    tree.addChild(note.getValueTree(), -1, undoManager);
}

std::vector<NoteState> MidiNoteExtractor::getNotes(const juce::ValueTree& tree)
{
    std::vector<NoteState> notes;
    if (!tree.hasType(resultTypeId))
        return notes;

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto child = tree.getChild(i);
        if (NoteState::isNote(child))
            notes.emplace_back(child);
    }

    return notes;
}
