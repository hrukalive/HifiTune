#include "PitchViewComponent.h"

PitchViewComponent::PitchViewComponent()
{
    setLookAndFeel(&lookAndFeel);

    amplitudeEnvelope.resize(128);
    pitchCurve.resize(128);

    for (size_t i = 0; i < amplitudeEnvelope.size(); ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(amplitudeEnvelope.size() - 1);
        amplitudeEnvelope[i] = 0.15f + 0.75f * std::sin(t * juce::MathConstants<float>::pi);
        pitchCurve[i] = 0.5f + 0.25f * std::sin(t * juce::MathConstants<float>::twoPi * 1.2f);
    }
}

PitchViewComponent::~PitchViewComponent()
{
    setLookAndFeel(nullptr);
}

void PitchViewComponent::setMode(EditorMode mode)
{
    currentMode = mode;
    repaint();
}

void PitchViewComponent::setAmplitudeEnvelope(const std::vector<float>& envelope)
{
    amplitudeEnvelope = envelope;
    repaint();
}

void PitchViewComponent::setPitchCurve(const std::vector<float>& curve)
{
    pitchCurve = curve;
    repaint();
}

void PitchViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    const int rows = 12;
    const float rowHeight = bounds.getHeight() / rows;

    for (int row = 0; row < rows; ++row)
    {
        const bool scaleTone = (row % 2 == 0);
        g.setColour(scaleTone ? juce::Colour::fromString("#1A1E2B") : juce::Colour::fromString("#141827"));
        g.fillRect(bounds.getX(), bounds.getY() + row * rowHeight, bounds.getWidth(), rowHeight);
    }

    if (!amplitudeEnvelope.empty())
    {
        juce::Path envelopePath;
        envelopePath.startNewSubPath(bounds.getX(), bounds.getBottom());

        for (size_t i = 0; i < amplitudeEnvelope.size(); ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(amplitudeEnvelope.size() - 1);
            const float x = bounds.getX() + t * bounds.getWidth();
            const float y = bounds.getBottom() - amplitudeEnvelope[i] * bounds.getHeight();
            envelopePath.lineTo(x, y);
        }

        envelopePath.lineTo(bounds.getRight(), bounds.getBottom());
        envelopePath.closeSubPath();
        g.setColour(juce::Colour::fromString("#2F8BBE").withAlpha(0.4f));
        g.fillPath(envelopePath);
    }

    if (!pitchCurve.empty())
    {
        juce::Path pitchPath;
        for (size_t i = 0; i < pitchCurve.size(); ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(pitchCurve.size() - 1);
            const float x = bounds.getX() + t * bounds.getWidth();
            const float y = bounds.getBottom() - pitchCurve[i] * bounds.getHeight();

            if (i == 0)
                pitchPath.startNewSubPath(x, y);
            else
                pitchPath.lineTo(x, y);
        }

        g.setColour(juce::Colour::fromString("#F5C27B"));
        g.strokePath(pitchPath, juce::PathStrokeType(2.0f));
    }

    g.setColour(juce::Colour::fromString("#C7CCE0"));
    g.setFont(13.0f);
    g.drawFittedText("Pitch View · Mode: " + toString(currentMode),
                     bounds.toNearestInt(),
                     juce::Justification::topLeft,
                     1);
}

void PitchViewComponent::resized()
{
}
