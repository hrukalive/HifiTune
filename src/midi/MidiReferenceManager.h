#pragma once

#include <juce_core/juce_core.h>

#include "MidiExtractor.h"
#include "../model/MidiReferenceState.h"
#include "../model/ProjectState.h"

class MidiReferenceManager
{
public:
    explicit MidiReferenceManager(ProjectState& projectState);

    juce::Array<MidiExtractor::TrackInfo> getTracks(const juce::File& midiFile, MidiExtractor& extractor) const;
    MidiReferenceState importReference(const juce::File& midiFile,
                                       int trackIndex,
                                       MidiExtractor& extractor,
                                       juce::UndoManager* undoManager);

    bool isSnapToReferenceEnabled() const;
    void setSnapToReferenceEnabled(bool enabled, juce::UndoManager* undoManager);

    bool isPitchAssignmentModeEnabled() const;
    void setPitchAssignmentModeEnabled(bool enabled, juce::UndoManager* undoManager);

    static const juce::Identifier snapToReferenceId;
    static const juce::Identifier pitchAssignmentModeId;

private:
    ProjectState& projectState;
};
