#include "MidiReferenceSnapper.h"

#include <limits>

MidiReferenceSnapper::MidiReferenceSnapper(const MidiReferenceState& reference)
    : notes(reference.getNotes())
{
}

std::optional<MidiReferenceSnapper::SnapResult> MidiReferenceSnapper::findNoteAtBeat(double beat) const
{
    for (const auto& note : notes)
    {
        const auto start = note.getStartBeat();
        const auto end = start + note.getLengthBeats();
        if (beat >= start && beat <= end)
        {
            return SnapResult{note.getMidiNote(), start, note.getLengthBeats(), note.getVelocity()};
        }
    }
    return std::nullopt;
}

std::optional<MidiReferenceSnapper::SnapResult> MidiReferenceSnapper::findClosestNote(double beat) const
{
    if (notes.empty())
        return std::nullopt;

    const MidiNoteState* closest = nullptr;
    double closestDistance = std::numeric_limits<double>::max();

    for (const auto& note : notes)
    {
        const auto start = note.getStartBeat();
        const auto end = start + note.getLengthBeats();
        const auto distance = beat < start ? start - beat : (beat > end ? beat - end : 0.0);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closest = &note;
        }
    }

    if (closest == nullptr)
        return std::nullopt;

    return SnapResult{closest->getMidiNote(),
                      closest->getStartBeat(),
                      closest->getLengthBeats(),
                      closest->getVelocity()};
}

int MidiReferenceSnapper::snapMidiNote(double beat, int fallbackMidiNote) const
{
    if (auto direct = findNoteAtBeat(beat))
        return direct->midiNote;

    if (auto closest = findClosestNote(beat))
        return closest->midiNote;

    return fallbackMidiNote;
}
