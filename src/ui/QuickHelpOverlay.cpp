#include "QuickHelpOverlay.h"

void QuickHelpOverlay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromString("#0B0D14").withAlpha(0.75f));

    g.setColour(juce::Colour::fromString("#1D2233"));
    g.fillRoundedRectangle(contentArea.toFloat(), 12.0f);

    g.setColour(juce::Colour::fromString("#E3E7FF"));
    g.setFont(18.0f);
    auto panelArea = contentArea;
    g.drawText("Quick Help", panelArea.removeFromTop(36), juce::Justification::centredLeft);

    g.setColour(juce::Colour::fromString("#AAB2D8"));
    g.setFont(13.5f);
    auto lineArea = panelArea.reduced(12, 8);
    for (const auto& line : shortcutLines)
    {
        g.drawText(line, lineArea.removeFromTop(22), juce::Justification::centredLeft);
    }

    g.setColour(juce::Colour::fromString("#7C83A8"));
    g.setFont(11.0f);
    g.drawText("Click anywhere to close", panelArea.removeFromBottom(22), juce::Justification::centredRight);
}

void QuickHelpOverlay::resized()
{
    const auto bounds = getLocalBounds();
    const int width = juce::jmin(520, bounds.getWidth() - 40);
    const int height = juce::jmin(360, bounds.getHeight() - 40);
    contentArea = juce::Rectangle<int>(width, height).withCentre(bounds.getCentre());
}

void QuickHelpOverlay::mouseDown(const juce::MouseEvent&)
{
    if (onDismiss)
        onDismiss();
}

void QuickHelpOverlay::setShortcuts(const juce::StringArray& shortcuts)
{
    shortcutLines = shortcuts;
    repaint();
}
