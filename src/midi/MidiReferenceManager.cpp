#include "MidiReferenceManager.h"

const juce::Identifier MidiReferenceManager::snapToReferenceId{"snapToMidiReference"};
const juce::Identifier MidiReferenceManager::pitchAssignmentModeId{"pitchToNoteAssignmentMode"};

MidiReferenceManager::MidiReferenceManager(ProjectState& projectStateIn)
    : projectState(projectStateIn)
{
}

juce::Array<MidiExtractor::TrackInfo> MidiReferenceManager::getTracks(const juce::File& midiFile,
                                                                      MidiExtractor& extractor) const
{
    return extractor.getTracks(midiFile);
}

MidiReferenceState MidiReferenceManager::importReference(const juce::File& midiFile,
                                                         int trackIndex,
                                                         MidiExtractor& extractor,
                                                         juce::UndoManager* undoManager)
{
    auto referenceTree = projectState.getMidiReferenceTree();
    MidiReferenceState reference(referenceTree);

    auto tracks = extractor.getTracks(midiFile);
    juce::String trackName;
    for (const auto& track : tracks)
    {
        if (track.index == trackIndex)
        {
            trackName = track.name;
            break;
        }
    }
    if (trackName.isEmpty())
        trackName = "Track " + juce::String(trackIndex + 1);

    reference.setFilePath(midiFile.getFullPathName(), undoManager);
    reference.setTrackIndex(trackIndex, undoManager);
    reference.setTrackName(trackName, undoManager);

    auto notes = extractor.extractNotes(midiFile, trackIndex);
    reference.setNotes(notes, undoManager);
    return reference;
}

bool MidiReferenceManager::isSnapToReferenceEnabled() const
{
    return static_cast<bool>(projectState.getSettingsTree().getProperty(snapToReferenceId, false));
}

void MidiReferenceManager::setSnapToReferenceEnabled(bool enabled, juce::UndoManager* undoManager)
{
    projectState.getSettingsTree().setProperty(snapToReferenceId, enabled, undoManager);
}

bool MidiReferenceManager::isPitchAssignmentModeEnabled() const
{
    return static_cast<bool>(projectState.getSettingsTree().getProperty(pitchAssignmentModeId, false));
}

void MidiReferenceManager::setPitchAssignmentModeEnabled(bool enabled, juce::UndoManager* undoManager)
{
    projectState.getSettingsTree().setProperty(pitchAssignmentModeId, enabled, undoManager);
}
