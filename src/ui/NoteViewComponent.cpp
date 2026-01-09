#include "NoteViewComponent.h"

NoteViewComponent::NoteViewComponent()
{
    setLookAndFeel(&lookAndFeel);

    notes = {
        { 0.0, 1.0, 60, false, true },
        { 1.25, 0.75, 62, false, true },
        { 2.2, 1.4, 64, false, true },
        { 3.9, 0.8, 65, true, false },
        { 4.8, 1.2, 67, false, true },
        { 6.1, 1.3, 69, false, false }
    };
}

NoteViewComponent::~NoteViewComponent()
{
    setLookAndFeel(nullptr);
}

void NoteViewComponent::setMode(EditorMode mode)
{
    currentMode = mode;
    repaint();
}

void NoteViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    const auto noteArea = getNoteArea();
    const int rows = 24;
    const float rowHeight = noteArea.getHeight() / rows;

    for (int row = 0; row < rows; ++row)
    {
        const int midiNote = 84 - row;
        const float y = noteArea.getY() + row * rowHeight;
        juce::Rectangle<float> rowRect(noteArea.getX(), y, noteArea.getWidth(), rowHeight);

        if (isScaleTone(midiNote))
            g.setColour(juce::Colour::fromString("#1C2030"));
        else
            g.setColour(juce::Colour::fromString("#141723"));

        g.fillRect(rowRect);
    }

    g.setColour(juce::Colour::fromString("#2A2E42"));
    for (int row = 0; row <= rows; ++row)
    {
        const float y = noteArea.getY() + row * rowHeight;
        g.drawLine(noteArea.getX(), y, noteArea.getRight(), y, 0.5f);
    }

    const double viewBeats = 8.0;
    for (int beat = 0; beat <= 8; ++beat)
    {
        const float x = noteArea.getX() + static_cast<float>((beat / viewBeats) * noteArea.getWidth());
        g.setColour(beat % 4 == 0 ? juce::Colour::fromString("#3B405A") : juce::Colour::fromString("#24283B"));
        g.drawLine(x, noteArea.getY(), x, noteArea.getBottom(), 1.0f);
    }

    for (size_t i = 0; i < notes.size(); ++i)
    {
        const auto& note = notes[i];
        auto bounds = getNoteBounds(note, noteArea);
        const bool selected = static_cast<int>(i) == selectedIndex;

        if (selected)
            g.setColour(juce::Colour::fromString("#5ED4FF"));
        else if (note.temporary)
            g.setColour(juce::Colour::fromString("#F6C177"));
        else if (note.assigned)
            g.setColour(juce::Colour::fromString("#8AADF4"));
        else
            g.setColour(juce::Colour::fromString("#7E84A3"));

        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colour::fromString("#0B0C10"));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

    auto barArea = getLocalBounds().toFloat().removeFromTop(24.0f);
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(notes.size()))
    {
        g.setColour(juce::Colour::fromString("#5ED4FF"));
        g.fillRect(barArea);
        g.setColour(juce::Colour::fromString("#0B0C10"));
        g.setFont(13.0f);
        g.drawFittedText("Selected Note: " + juce::String(notes[static_cast<size_t>(selectedIndex)].pitch)
                             + " | Mode: " + toString(currentMode),
                         barArea.toNearestInt(),
                         juce::Justification::centredLeft,
                         1);
    }
    else
    {
        g.setColour(juce::Colour::fromString("#1B1F2D"));
        g.fillRect(barArea);
        g.setColour(juce::Colour::fromString("#8F94B2"));
        g.setFont(12.0f);
        g.drawFittedText("Click a note to select. Mode: " + toString(currentMode),
                         barArea.toNearestInt(),
                         juce::Justification::centredLeft,
                         1);
    }
}

void NoteViewComponent::resized()
{
}

void NoteViewComponent::mouseDown(const juce::MouseEvent& event)
{
    const auto noteArea = getNoteArea();
    selectedIndex = -1;

    for (size_t i = 0; i < notes.size(); ++i)
    {
        if (getNoteBounds(notes[i], noteArea).contains(event.position))
        {
            selectedIndex = static_cast<int>(i);
            break;
        }
    }

    repaint();
}

juce::Rectangle<float> NoteViewComponent::getNoteArea() const
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    bounds.removeFromTop(24.0f);
    return bounds;
}

juce::Rectangle<float> NoteViewComponent::getNoteBounds(const Note& note, const juce::Rectangle<float>& area) const
{
    const double viewBeats = 8.0;
    const int rows = 24;
    const float rowHeight = area.getHeight() / rows;

    const float x = area.getX() + static_cast<float>((note.startBeat / viewBeats) * area.getWidth());
    const float width = static_cast<float>((note.durationBeats / viewBeats) * area.getWidth());
    const int row = juce::jlimit(0, rows - 1, 84 - note.pitch);
    const float y = area.getY() + row * rowHeight;

    return { x + 2.0f, y + 2.0f, width - 4.0f, rowHeight - 4.0f };
}

bool NoteViewComponent::isScaleTone(int midiNote) const
{
    static const int scaleDegrees[] = { 0, 2, 4, 5, 7, 9, 11 };
    const int degree = midiNote % 12;
    for (int scaleDegree : scaleDegrees)
    {
        if (degree == scaleDegree)
            return true;
    }

    return false;
}
