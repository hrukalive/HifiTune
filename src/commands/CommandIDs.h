#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace CommandIDs
{
    static constexpr juce::CommandID undo = 0x1000;
    static constexpr juce::CommandID redo = 0x1001;

    static constexpr juce::CommandID selectMode = 0x1100;
    static constexpr juce::CommandID drawMode = 0x1101;
    static constexpr juce::CommandID eraseMode = 0x1102;
    static constexpr juce::CommandID pitchMode = 0x1103;

    static constexpr juce::CommandID togglePianoRollView = 0x1200;
    static constexpr juce::CommandID toggleMixerView = 0x1201;
    static constexpr juce::CommandID toggleAutomationView = 0x1202;
    static constexpr juce::CommandID togglePitchCurveView = 0x1203;
    static constexpr juce::CommandID toggleTensionView = 0x1204;

    static constexpr juce::CommandID autoZoomSelection = 0x1300;
    static constexpr juce::CommandID autoZoomAll = 0x1301;

    static constexpr juce::CommandID previewNote = 0x1400;
    static constexpr juce::CommandID previewTrack = 0x1401;

    static constexpr juce::CommandID toggleSnap = 0x1500;
    static constexpr juce::CommandID toggleMetronome = 0x1501;
    static constexpr juce::CommandID toggleLoop = 0x1502;
    static constexpr juce::CommandID toggleAutoScroll = 0x1503;
}
