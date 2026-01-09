#include "NoteViewComponent.h"
#include <cmath>

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

    viewport.setMinimumViewSize(0.5, 1.0);
    updateContentRanges();
    viewport.fitToContent();
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

void NoteViewComponent::setAutoFollow(bool enabled)
{
    updateAutoFollowState(enabled, true);
}

void NoteViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    const auto noteArea = getNoteArea();
    const auto viewX = viewport.getViewX();
    const auto viewY = viewport.getViewY();
    const double viewHeight = viewY.getLength();
    const int maxNote = static_cast<int>(std::ceil(viewY.getEnd()));
    const int minNote = static_cast<int>(std::floor(viewY.getStart()));

    for (int note = maxNote; note >= minNote; --note)
    {
        const float yTop = noteArea.getY()
                           + static_cast<float>(((viewY.getEnd() - note) / viewHeight) * noteArea.getHeight());
        const float yBottom = noteArea.getY()
                              + static_cast<float>(((viewY.getEnd() - (note - 1)) / viewHeight) * noteArea.getHeight());
        const float height = juce::jmax(1.0f, yBottom - yTop);
        juce::Rectangle<float> rowRect(noteArea.getX(), yTop, noteArea.getWidth(), height);

        if (isScaleTone(note))
            g.setColour(juce::Colour::fromString("#1C2030"));
        else
            g.setColour(juce::Colour::fromString("#141723"));

        g.fillRect(rowRect);
    }

    g.setColour(juce::Colour::fromString("#2A2E42"));
    for (int note = maxNote; note >= minNote; --note)
    {
        const float y = noteArea.getY()
                        + static_cast<float>(((viewY.getEnd() - note) / viewHeight) * noteArea.getHeight());
        g.drawLine(noteArea.getX(), y, noteArea.getRight(), y, 0.5f);
    }

    const int startBeat = static_cast<int>(std::floor(viewX.getStart()));
    const int endBeat = static_cast<int>(std::ceil(viewX.getEnd()));
    for (int beat = startBeat; beat <= endBeat; ++beat)
    {
        const float x = noteArea.getX()
                        + static_cast<float>(((beat - viewX.getStart()) / viewX.getLength()) * noteArea.getWidth());
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

    auto barArea = getLocalBounds().toFloat().removeFromTop(26.0f);
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
        g.setColour(juce::Colour::fromString("#B6BCD6"));
        g.setFont(12.0f);
        g.drawFittedText("Note Bar · Mode: " + toString(currentMode) + " · Click a note to select",
                         barArea.toNearestInt(),
                         juce::Justification::centredLeft,
                         1);
    }
}

void NoteViewComponent::resized()
{
    if (autoFollowEnabled)
        viewport.fitToContent();
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

void NoteViewComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (viewport.handleWheel(wheel, event.mods))
    {
        updateAutoFollowState(false, false);
        repaint();
    }
}

juce::Rectangle<float> NoteViewComponent::getNoteArea() const
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    bounds.removeFromTop(26.0f);
    return bounds;
}

juce::Rectangle<float> NoteViewComponent::getNoteBounds(const Note& note, const juce::Rectangle<float>& area) const
{
    const auto viewX = viewport.getViewX();
    const auto viewY = viewport.getViewY();
    const double viewHeight = viewY.getLength();

    const float x = area.getX()
                    + static_cast<float>(((note.startBeat - viewX.getStart()) / viewX.getLength()) * area.getWidth());
    const float width = static_cast<float>((note.durationBeats / viewX.getLength()) * area.getWidth());
    const float y = area.getY()
                    + static_cast<float>(((viewY.getEnd() - note.pitch) / viewHeight) * area.getHeight());
    const float height = static_cast<float>(area.getHeight() / viewHeight);

    return { x + 2.0f, y + 2.0f, width - 4.0f, height - 4.0f };
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

void NoteViewComponent::updateContentRanges()
{
    if (notes.empty())
    {
        viewport.setContentRanges({ 0.0, 8.0 }, { 60.0, 84.0 });
        return;
    }

    double minBeat = notes.front().startBeat;
    double maxBeat = notes.front().startBeat + notes.front().durationBeats;
    int minPitch = notes.front().pitch;
    int maxPitch = notes.front().pitch;

    for (const auto& note : notes)
    {
        minBeat = juce::jmin(minBeat, note.startBeat);
        maxBeat = juce::jmax(maxBeat, note.startBeat + note.durationBeats);
        minPitch = juce::jmin(minPitch, note.pitch);
        maxPitch = juce::jmax(maxPitch, note.pitch);
    }

    viewport.setContentRanges({ minBeat, maxBeat }, { static_cast<double>(minPitch), static_cast<double>(maxPitch + 1) });

    if (autoFollowEnabled)
        viewport.fitToContent();
}

void NoteViewComponent::updateAutoFollowState(bool enabled, bool refit)
{
    if (autoFollowEnabled == enabled)
        return;

    autoFollowEnabled = enabled;
    if (autoFollowEnabled && refit)
        viewport.fitToContent();

    if (onAutoFollowChanged)
        onAutoFollowChanged(autoFollowEnabled);
}
