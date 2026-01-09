#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "MidiNoteState.h"

class MidiReferenceState
{
public:
    explicit MidiReferenceState(juce::ValueTree tree);

    static MidiReferenceState create(const juce::String& filePath,
                                     int trackIndex,
                                     const juce::String& trackName);
    static bool isMidiReference(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    juce::String getFilePath() const;
    int getTrackIndex() const;
    juce::String getTrackName() const;

    void setFilePath(const juce::String& filePath, juce::UndoManager* undoManager);
    void setTrackIndex(int trackIndex, juce::UndoManager* undoManager);
    void setTrackName(const juce::String& trackName, juce::UndoManager* undoManager);

    juce::ValueTree getNotesTree();
    std::vector<MidiNoteState> getNotes() const;
    void setNotes(const std::vector<MidiNoteState>& notes, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier filePathId;
    static const juce::Identifier trackIndexId;
    static const juce::Identifier trackNameId;
    static const juce::Identifier notesTypeId;

private:
    juce::ValueTree ensureNotesTree();

    juce::ValueTree state;
};
