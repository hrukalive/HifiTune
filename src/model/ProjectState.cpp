#include "ProjectState.h"

const juce::Identifier ProjectState::projectTypeId{"Project"};
const juce::Identifier ProjectState::tracksTypeId{"Tracks"};
const juce::Identifier ProjectState::clipsTypeId{"Clips"};
const juce::Identifier ProjectState::segmentsTypeId{"Segments"};
const juce::Identifier ProjectState::notesTypeId{"Notes"};
const juce::Identifier ProjectState::editsTypeId{"Edits"};
const juce::Identifier ProjectState::settingsTypeId{"Settings"};
const juce::Identifier ProjectState::sessionSettingsTypeId{"SessionSettings"};
const juce::Identifier ProjectState::midiReferenceTypeId{"MidiReference"};

ProjectState::Transaction::Transaction(ProjectState& stateIn, const juce::String& name)
    : state(&stateIn)
{
    state->beginTransaction(name);
}

ProjectState::Transaction::Transaction(Transaction&& other) noexcept
    : state(other.state)
{
    other.state = nullptr;
}

ProjectState::Transaction& ProjectState::Transaction::operator=(Transaction&& other) noexcept
{
    if (this != &other)
    {
        state = other.state;
        other.state = nullptr;
    }
    return *this;
}

ProjectState::Transaction::~Transaction() = default;

void ProjectState::Transaction::setName(const juce::String& name)
{
    if (state != nullptr)
        state->beginTransaction(name);
}

ProjectState::ProjectState()
    : ProjectState(juce::ValueTree(projectTypeId))
{
}

ProjectState::ProjectState(juce::ValueTree rootTree)
    : root(std::move(rootTree))
{
    if (!root.isValid())
        root = juce::ValueTree(projectTypeId);

    if (root.getType() != projectTypeId)
        root = juce::ValueTree(projectTypeId);

    ensureChildWithType(tracksTypeId);
    ensureChildWithType(clipsTypeId);
    ensureChildWithType(segmentsTypeId);
    ensureChildWithType(notesTypeId);
    ensureChildWithType(editsTypeId);
    ensureChildWithType(settingsTypeId);
    ensureChildWithType(sessionSettingsTypeId);
    ensureChildWithType(midiReferenceTypeId);
}

juce::ValueTree ProjectState::getRoot() const
{
    return root;
}

juce::ValueTree ProjectState::getTracksTree()
{
    return ensureChildWithType(tracksTypeId);
}

juce::ValueTree ProjectState::getClipsTree()
{
    return ensureChildWithType(clipsTypeId);
}

juce::ValueTree ProjectState::getSegmentsTree()
{
    return ensureChildWithType(segmentsTypeId);
}

juce::ValueTree ProjectState::getNotesTree()
{
    return ensureChildWithType(notesTypeId);
}

juce::ValueTree ProjectState::getEditsTree()
{
    return ensureChildWithType(editsTypeId);
}

juce::ValueTree ProjectState::getSettingsTree()
{
    return ensureChildWithType(settingsTypeId);
}

juce::ValueTree ProjectState::getSessionSettingsTree()
{
    return ensureChildWithType(sessionSettingsTypeId);
}

juce::ValueTree ProjectState::getMidiReferenceTree()
{
    return ensureChildWithType(midiReferenceTypeId);
}

juce::UndoManager& ProjectState::getUndoManager()
{
    return undoManager;
}

void ProjectState::addListener(juce::ValueTree::Listener* listener)
{
    root.addListener(listener);
}

void ProjectState::removeListener(juce::ValueTree::Listener* listener)
{
    root.removeListener(listener);
}

void ProjectState::beginTransaction(const juce::String& name)
{
    undoManager.beginNewTransaction(name);
}

ProjectState::Transaction ProjectState::startTransaction(const juce::String& name)
{
    return Transaction(*this, name);
}

juce::ValueTree ProjectState::ensureChildWithType(const juce::Identifier& type)
{
    auto child = root.getChildWithName(type);
    if (!child.isValid())
    {
        child = juce::ValueTree(type);
        root.addChild(child, -1, &undoManager);
    }
    return child;
}
