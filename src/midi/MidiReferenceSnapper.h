#pragma once

#include <optional>

#include "../model/MidiReferenceState.h"

class MidiReferenceSnapper
{
public:
    struct SnapResult
    {
        int midiNote = 0;
        double startBeat = 0.0;
        double lengthBeats = 0.0;
        double velocity = 0.0;
    };

    explicit MidiReferenceSnapper(const MidiReferenceState& reference);

    std::optional<SnapResult> findNoteAtBeat(double beat) const;
    std::optional<SnapResult> findClosestNote(double beat) const;
    int snapMidiNote(double beat, int fallbackMidiNote) const;

private:
    std::vector<MidiNoteState> notes;
};
