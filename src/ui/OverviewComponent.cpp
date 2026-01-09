#include "OverviewComponent.h"

OverviewComponent::OverviewComponent()
{
    setLookAndFeel(&lookAndFeel);
}

OverviewComponent::~OverviewComponent()
{
    setLookAndFeel(nullptr);
}

void OverviewComponent::setViewRange(double start, double end)
{
    viewStart = start;
    viewEnd = end;
    repaint();
}

void OverviewComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    g.setColour(juce::Colour::fromString("#2E3350"));
    g.fillRoundedRectangle(bounds, 6.0f);

    auto waveformBounds = bounds.reduced(6.0f);
    juce::Path wave;
    wave.startNewSubPath(waveformBounds.getX(), waveformBounds.getCentreY());
    for (int i = 0; i <= 32; ++i)
    {
        const float t = static_cast<float>(i) / 32.0f;
        const float x = waveformBounds.getX() + t * waveformBounds.getWidth();
        const float y = waveformBounds.getCentreY() + std::sin(t * juce::MathConstants<float>::twoPi) * waveformBounds.getHeight() * 0.25f;
        wave.lineTo(x, y);
    }

    g.setColour(juce::Colour::fromString("#9AA0C4"));
    g.strokePath(wave, juce::PathStrokeType(1.5f));

    const double totalRange = 8.0;
    const float startX = waveformBounds.getX() + static_cast<float>((viewStart / totalRange) * waveformBounds.getWidth());
    const float endX = waveformBounds.getX() + static_cast<float>((viewEnd / totalRange) * waveformBounds.getWidth());

    juce::Rectangle<float> viewRect(startX, waveformBounds.getY(), endX - startX, waveformBounds.getHeight());
    g.setColour(juce::Colour::fromString("#5ED4FF").withAlpha(0.25f));
    g.fillRoundedRectangle(viewRect, 4.0f);
    g.setColour(juce::Colour::fromString("#5ED4FF"));
    g.drawRoundedRectangle(viewRect, 4.0f, 1.2f);
}

void OverviewComponent::resized()
{
}
