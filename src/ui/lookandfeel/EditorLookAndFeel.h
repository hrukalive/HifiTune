#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class EditorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    EditorLookAndFeel();
};

class ToolbarLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ToolbarLookAndFeel();
};

class SidebarLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SidebarLookAndFeel();
};

class NoteViewLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NoteViewLookAndFeel();
};

class PitchViewLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PitchViewLookAndFeel();
};

class OverviewLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OverviewLookAndFeel();
};

class TimelineLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TimelineLookAndFeel();
};
