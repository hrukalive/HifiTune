#pragma once

#include <juce_data_structures/juce_data_structures.h>

class ProjectState final
{
public:
    class Transaction
    {
    public:
        Transaction(ProjectState& state, const juce::String& name);
        Transaction(const Transaction&) = delete;
        Transaction& operator=(const Transaction&) = delete;
        Transaction(Transaction&& other) noexcept;
        Transaction& operator=(Transaction&& other) noexcept;
        ~Transaction();

        void setName(const juce::String& name);

    private:
        ProjectState* state = nullptr;
    };

    ProjectState();
    explicit ProjectState(juce::ValueTree rootTree);

    juce::ValueTree getRoot() const;

    juce::ValueTree getTracksTree();
    juce::ValueTree getClipsTree();
    juce::ValueTree getSegmentsTree();
    juce::ValueTree getNotesTree();
    juce::ValueTree getEditsTree();
    juce::ValueTree getSettingsTree();
    juce::ValueTree getSessionSettingsTree();

    juce::UndoManager& getUndoManager();

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    void beginTransaction(const juce::String& name);
    Transaction startTransaction(const juce::String& name);

    static const juce::Identifier projectTypeId;
    static const juce::Identifier tracksTypeId;
    static const juce::Identifier clipsTypeId;
    static const juce::Identifier segmentsTypeId;
    static const juce::Identifier notesTypeId;
    static const juce::Identifier editsTypeId;
    static const juce::Identifier settingsTypeId;
    static const juce::Identifier sessionSettingsTypeId;

private:
    juce::ValueTree ensureChildWithType(const juce::Identifier& type);

    juce::ValueTree root;
    juce::UndoManager undoManager;
};
