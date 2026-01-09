#pragma once

#include <juce_core/juce_core.h>

enum class EditorMode
{
    PitchAmplitude,
    Timing,
    PitchDraw,
    TemporaryNote,
    NoteAssignment,
    Harmony
};

inline juce::String toString(EditorMode mode)
{
    switch (mode)
    {
        case EditorMode::PitchAmplitude:
            return "Pitch/Amplitude";
        case EditorMode::Timing:
            return "Timing";
        case EditorMode::PitchDraw:
            return "Pitch Draw";
        case EditorMode::TemporaryNote:
            return "Temporary Note";
        case EditorMode::NoteAssignment:
            return "Note Assignment";
        case EditorMode::Harmony:
            return "Harmony";
    }

    return "Unknown";
}
