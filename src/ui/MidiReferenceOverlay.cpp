#include "MidiReferenceOverlay.h"

void MidiReferenceOverlay::setReferenceNotes(std::vector<MidiNoteState> notes)
{
    referenceNotes = std::move(notes);
    repaint();
}

void MidiReferenceOverlay::setViewRange(double startBeat,
                                        double endBeat,
                                        int lowestMidiNote,
                                        int highestMidiNote)
{
    viewStartBeat = startBeat;
    viewEndBeat = endBeat;
    viewLowestNote = lowestMidiNote;
    viewHighestNote = highestMidiNote;
    repaint();
}

void MidiReferenceOverlay::setOverlayColour(juce::Colour colour)
{
    overlayColour = colour;
    repaint();
}

void MidiReferenceOverlay::paint(juce::Graphics& g)
{
    const auto width = static_cast<float>(getWidth());
    const auto height = static_cast<float>(getHeight());

    const auto beatSpan = viewEndBeat - viewStartBeat;
    const auto noteSpan = viewHighestNote - viewLowestNote + 1;
    if (beatSpan <= 0.0 || noteSpan <= 0)
        return;

    g.setColour(overlayColour);

    for (const auto& note : referenceNotes)
    {
        const auto noteStart = note.getStartBeat();
        const auto noteEnd = noteStart + note.getLengthBeats();
        if (noteEnd < viewStartBeat || noteStart > viewEndBeat)
            continue;

        const auto midiNote = note.getMidiNote();
        if (midiNote < viewLowestNote || midiNote > viewHighestNote)
            continue;

        const auto x = static_cast<float>(((noteStart - viewStartBeat) / beatSpan) * width);
        const auto noteWidth = static_cast<float>((note.getLengthBeats() / beatSpan) * width);

        const auto noteIndex = viewHighestNote - midiNote;
        const auto rowHeight = height / static_cast<float>(noteSpan);
        const auto y = rowHeight * static_cast<float>(noteIndex);

        g.fillRect(juce::Rectangle<float>(x, y, std::max(1.0f, noteWidth), rowHeight));
    }
}
