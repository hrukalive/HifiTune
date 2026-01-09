#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EditorMode.h"
#include "ViewportController.h"
#include "lookandfeel/EditorLookAndFeel.h"

class NoteViewComponent final : public juce::Component
{
public:
    NoteViewComponent();
    ~NoteViewComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void setMode(EditorMode mode);
    void setAutoFollow(bool enabled);

    std::function<void(bool)> onAutoFollowChanged;

private:
    struct Note
    {
        double startBeat = 0.0;
        double durationBeats = 1.0;
        int pitch = 60;
        bool temporary = false;
        bool assigned = false;
    };

    juce::Rectangle<float> getNoteArea() const;
    juce::Rectangle<float> getNoteBounds(const Note& note, const juce::Rectangle<float>& area) const;
    bool isScaleTone(int midiNote) const;
    void updateContentRanges();
    void updateAutoFollowState(bool enabled, bool refit);

    NoteViewLookAndFeel lookAndFeel;
    std::vector<Note> notes;
    ViewportController viewport;
    bool autoFollowEnabled = true;
    int selectedIndex = -1;
    EditorMode currentMode { EditorMode::PitchAmplitude };
};
