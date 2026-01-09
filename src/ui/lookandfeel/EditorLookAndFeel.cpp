#include "EditorLookAndFeel.h"

EditorLookAndFeel::EditorLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#11131C"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#F5F7FF"));
}

ToolbarLookAndFeel::ToolbarLookAndFeel()
{
    setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#24283B"));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromString("#5ED4FF"));
    setColour(juce::TextButton::textColourOnId, juce::Colour::fromString("#0B0C10"));
    setColour(juce::TextButton::textColourOffId, juce::Colour::fromString("#DADFF7"));
}

SidebarLookAndFeel::SidebarLookAndFeel()
{
    setColour(juce::GroupComponent::outlineColourId, juce::Colour::fromString("#2C3045"));
    setColour(juce::GroupComponent::textColourId, juce::Colour::fromString("#F5F7FF"));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromString("#1B1E2C"));
    setColour(juce::ComboBox::textColourId, juce::Colour::fromString("#F5F7FF"));
}

NoteViewLookAndFeel::NoteViewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#121421"));
}

PitchViewLookAndFeel::PitchViewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#111521"));
}

OverviewLookAndFeel::OverviewLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#0E1018"));
}

TimelineLookAndFeel::TimelineLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromString("#0F111A"));
    setColour(juce::Label::textColourId, juce::Colour::fromString("#C7CCE0"));
}
