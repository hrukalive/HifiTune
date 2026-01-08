#include "LookAndFeel.h"

HifiTuneLookAndFeel::HifiTuneLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#12131A"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::Slider::trackColourId, juce::Colour::fromString("#5ED4FF"));
}
