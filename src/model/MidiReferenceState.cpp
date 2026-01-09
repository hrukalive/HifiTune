#include "MidiReferenceState.h"

const juce::Identifier MidiReferenceState::typeId{"MidiReference"};
const juce::Identifier MidiReferenceState::filePathId{"filePath"};
const juce::Identifier MidiReferenceState::trackIndexId{"trackIndex"};
const juce::Identifier MidiReferenceState::trackNameId{"trackName"};
const juce::Identifier MidiReferenceState::notesTypeId{"MidiReferenceNotes"};

MidiReferenceState::MidiReferenceState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

MidiReferenceState MidiReferenceState::create(const juce::String& filePath,
                                             int trackIndex,
                                             const juce::String& trackName)
{
    juce::ValueTree tree(typeId);
    tree.setProperty(filePathId, filePath, nullptr);
    tree.setProperty(trackIndexId, trackIndex, nullptr);
    tree.setProperty(trackNameId, trackName, nullptr);
    tree.addChild(juce::ValueTree(notesTypeId), -1, nullptr);
    return MidiReferenceState(tree);
}

bool MidiReferenceState::isMidiReference(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

bool MidiReferenceState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree MidiReferenceState::getValueTree() const
{
    return state;
}

juce::String MidiReferenceState::getFilePath() const
{
    return state.getProperty(filePathId).toString();
}

int MidiReferenceState::getTrackIndex() const
{
    return static_cast<int>(state.getProperty(trackIndexId, -1));
}

juce::String MidiReferenceState::getTrackName() const
{
    return state.getProperty(trackNameId).toString();
}

void MidiReferenceState::setFilePath(const juce::String& filePath, juce::UndoManager* undoManager)
{
    state.setProperty(filePathId, filePath, undoManager);
}

void MidiReferenceState::setTrackIndex(int trackIndex, juce::UndoManager* undoManager)
{
    state.setProperty(trackIndexId, trackIndex, undoManager);
}

void MidiReferenceState::setTrackName(const juce::String& trackName, juce::UndoManager* undoManager)
{
    state.setProperty(trackNameId, trackName, undoManager);
}

juce::ValueTree MidiReferenceState::getNotesTree()
{
    return ensureNotesTree();
}

std::vector<MidiNoteState> MidiReferenceState::getNotes() const
{
    std::vector<MidiNoteState> notes;
    auto notesTree = state.getChildWithName(notesTypeId);
    if (!notesTree.isValid())
        return notes;

    notes.reserve(static_cast<size_t>(notesTree.getNumChildren()));
    for (int i = 0; i < notesTree.getNumChildren(); ++i)
    {
        auto child = notesTree.getChild(i);
        if (MidiNoteState::isNote(child))
            notes.emplace_back(child);
    }

    return notes;
}

void MidiReferenceState::setNotes(const std::vector<MidiNoteState>& notes, juce::UndoManager* undoManager)
{
    auto notesTree = ensureNotesTree();
    for (int i = notesTree.getNumChildren(); --i >= 0;)
        notesTree.removeChild(i, undoManager);

    for (const auto& note : notes)
    {
        if (note.isValid())
            notesTree.addChild(note.getValueTree(), -1, undoManager);
    }
}

void MidiReferenceState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void MidiReferenceState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}

juce::ValueTree MidiReferenceState::ensureNotesTree()
{
    auto notesTree = state.getChildWithName(notesTypeId);
    if (!notesTree.isValid())
    {
        notesTree = juce::ValueTree(notesTypeId);
        state.addChild(notesTree, -1, nullptr);
    }
    return notesTree;
}
