#include "PitchViewComponent.h"

PitchViewComponent::PitchViewComponent()
{
    setLookAndFeel(&lookAndFeel);

    amplitudeEnvelope.resize(128);
    pitchCurve.resize(128);
    viewport.setMinimumViewSize(0.05, 0.05);

    for (size_t i = 0; i < amplitudeEnvelope.size(); ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(amplitudeEnvelope.size() - 1);
        amplitudeEnvelope[i] = 0.15f + 0.75f * std::sin(t * juce::MathConstants<float>::pi);
        pitchCurve[i] = 0.5f + 0.25f * std::sin(t * juce::MathConstants<float>::twoPi * 1.2f);
    }

    updateContentRanges();
    viewport.fitToContent();
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
    updateContentRanges();
    repaint();
}

void PitchViewComponent::setPitchCurve(const std::vector<float>& curve)
{
    pitchCurve = curve;
    updateContentRanges();
    repaint();
}

void PitchViewComponent::setAutoFollow(bool enabled)
{
    updateAutoFollowState(enabled, true);
}

void PitchViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    const auto viewX = viewport.getViewX();
    const auto viewY = viewport.getViewY();
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
            const double dataX = t;
            const double dataY = amplitudeEnvelope[i];
            const float x = bounds.getX()
                            + static_cast<float>(((dataX - viewX.getStart()) / viewX.getLength()) * bounds.getWidth());
            const float y = bounds.getBottom()
                            - static_cast<float>(((dataY - viewY.getStart()) / viewY.getLength()) * bounds.getHeight());
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
            const double dataX = t;
            const double dataY = pitchCurve[i];
            const float x = bounds.getX()
                            + static_cast<float>(((dataX - viewX.getStart()) / viewX.getLength()) * bounds.getWidth());
            const float y = bounds.getBottom()
                            - static_cast<float>(((dataY - viewY.getStart()) / viewY.getLength()) * bounds.getHeight());

            if (i == 0)
                pitchPath.startNewSubPath(x, y);
            else
                pitchPath.lineTo(x, y);
        }

        g.setColour(juce::Colour::fromString("#F5C27B"));
        g.strokePath(pitchPath, juce::PathStrokeType(2.0f));
    }

    g.setColour(juce::Colour::fromString("#E3E7FF"));
    g.setFont(13.0f);
    g.drawText("Pitch View", bounds.removeFromTop(18).toNearestInt(), juce::Justification::centredLeft);
    g.setColour(juce::Colour::fromString("#8F94B2"));
    g.setFont(11.0f);
    g.drawText("Mode: " + toString(currentMode), bounds.removeFromTop(16).toNearestInt(), juce::Justification::centredLeft);
}

void PitchViewComponent::resized()
{
    if (autoFollowEnabled)
        viewport.fitToContent();
}

void PitchViewComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (viewport.handleWheel(wheel, event.mods))
    {
        updateAutoFollowState(false, false);
        repaint();
    }
}

void PitchViewComponent::updateContentRanges()
{
    if (pitchCurve.empty())
    {
        viewport.setContentRanges({ 0.0, 1.0 }, { 0.0, 1.0 });
        return;
    }

    double minPitch = pitchCurve.front();
    double maxPitch = pitchCurve.front();
    for (float value : pitchCurve)
    {
        minPitch = juce::jmin(minPitch, static_cast<double>(value));
        maxPitch = juce::jmax(maxPitch, static_cast<double>(value));
    }

    viewport.setContentRanges({ 0.0, 1.0 }, { minPitch, maxPitch });

    if (autoFollowEnabled)
        viewport.fitToContent();
}

void PitchViewComponent::updateAutoFollowState(bool enabled, bool refit)
{
    if (autoFollowEnabled == enabled)
        return;

    autoFollowEnabled = enabled;
    if (autoFollowEnabled && refit)
        viewport.fitToContent();

    if (onAutoFollowChanged)
        onAutoFollowChanged(autoFollowEnabled);
}
