#include "EditorLookAndFeel.h"

EditorLookAndFeel::EditorLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#11131C"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#262C3E"));
    setColour(juce::TextButton::textColourOffId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::GroupComponent::textColourId, juce::Colour::fromString("#E3E7FF"));
    setColour(juce::ComboBox::textColourId, juce::Colour::fromString("#F5F7FF"));
}

ToolbarLookAndFeel::ToolbarLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#191D2B"));
    setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#2A3043"));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromString("#5ED4FF"));
    setColour(juce::TextButton::textColourOnId, juce::Colour::fromString("#0B0C10"));
    setColour(juce::TextButton::textColourOffId, juce::Colour::fromString("#F0F2FF"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#C7CCE0"));
}

SidebarLookAndFeel::SidebarLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#171B28"));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour::fromString("#3A3F55"));
    setColour(juce::GroupComponent::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromString("#23283A"));
    setColour(juce::ComboBox::outlineColourId, juce::Colour::fromString("#3A3F55"));
    setColour(juce::ComboBox::textColourId, juce::Colour::fromString("#F5F7FF"));
}

NoteViewLookAndFeel::NoteViewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#141829"));
}

PitchViewLookAndFeel::PitchViewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#131728"));
}

OverviewLookAndFeel::OverviewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#10131D"));
}

TimelineLookAndFeel::TimelineLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#121622"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#D2D6F0"));
}
