#include "LookAndFeel.h"
#include "lookandfeel/EditorLookAndFeel.h"

HifiTuneLookAndFeel::HifiTuneLookAndFeel()
{
    // Base backgrounds
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#12131A"));

    // Text
    setColour(juce::Label::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::ListBox::textColourId, juce::Colour::fromString("#E3E7FF"));

    // Controls
    setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#262C3E"));
    setColour(juce::TextButton::textColourOffId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::Slider::thumbColourId, juce::Colour::fromString("#5ED4FF"));
    setColour(juce::Slider::trackColourId, juce::Colour::fromString("#5ED4FF"));

    // Fallbacks for any other components
    setColour(juce::ComboBox::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::GroupComponent::textColourId, juce::Colour::fromString("#E3E7FF"));
}
