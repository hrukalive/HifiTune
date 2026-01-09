#include "TimelineComponent.h"

TimelineComponent::TimelineComponent()
{
    setLookAndFeel(&lookAndFeel);
}

TimelineComponent::~TimelineComponent()
{
    setLookAndFeel(nullptr);
}

void TimelineComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds().toFloat().reduced(8.0f, 4.0f);
    const int bars = 8;

    for (int bar = 0; bar <= bars; ++bar)
    {
        const float x = bounds.getX() + (static_cast<float>(bar) / bars) * bounds.getWidth();
        g.setColour(bar % 4 == 0 ? juce::Colour::fromString("#5ED4FF") : juce::Colour::fromString("#2C3045"));
        g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);

        if (bar < bars)
        {
            g.setColour(juce::Colour::fromString("#C7CCE0"));
            g.setFont(11.0f);
            g.drawText("" + juce::String(bar + 1),
                       juce::Rectangle<float>(x + 4.0f, bounds.getY(), 24.0f, bounds.getHeight()).toNearestInt(),
                       juce::Justification::topLeft);
        }
    }
}

void TimelineComponent::resized()
{
}
