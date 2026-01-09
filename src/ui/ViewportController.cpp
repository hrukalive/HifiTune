#include "ViewportController.h"
#include <cmath>

void ViewportController::setContentRanges(juce::Range<double> xRange, juce::Range<double> yRange)
{
    if (xRange.isEmpty())
        xRange = { 0.0, 1.0 };
    if (yRange.isEmpty())
        yRange = { 0.0, 1.0 };

    contentX = xRange;
    contentY = yRange;
    clampView();
}

void ViewportController::setMinimumViewSize(double minX, double minY)
{
    minViewX = minX;
    minViewY = minY;
    clampView();
}

void ViewportController::fitToContent(double paddingFactor)
{
    viewX = paddedRange(contentX, paddingFactor);
    viewY = paddedRange(contentY, paddingFactor);
    clampView();
}

bool ViewportController::handleWheel(const juce::MouseWheelDetails& details, const juce::ModifierKeys& modifiers)
{
    const double delta = details.deltaY != 0.0f ? details.deltaY : details.deltaX;
    if (delta == 0.0)
        return false;

    if (modifiers.isCtrlDown())
    {
        const double factor = std::pow(1.15, -delta * 5.0);
        viewX = zoomRange(viewX, factor);
    }
    else if (modifiers.isAltDown())
    {
        const double factor = std::pow(1.15, -delta * 5.0);
        viewY = zoomRange(viewY, factor);
    }
    else if (modifiers.isShiftDown())
    {
        const double offset = -delta * viewY.getLength() * 0.2;
        viewY = juce::Range<double>(viewY.getStart() + offset, viewY.getEnd() + offset);
    }
    else
    {
        const double offset = -delta * viewX.getLength() * 0.2;
        viewX = juce::Range<double>(viewX.getStart() + offset, viewX.getEnd() + offset);
    }

    clampView();
    return true;
}

void ViewportController::clampView()
{
    const double minXLength = juce::jmax(minViewX, 1.0e-6);
    const double minYLength = juce::jmax(minViewY, 1.0e-6);

    const auto ensureMinLength = [](const juce::Range<double>& range, double minLength)
    {
        if (range.getLength() >= minLength)
            return range;
        const double centre = range.getStart() + range.getLength() * 0.5;
        return juce::Range<double>(centre - minLength * 0.5, centre + minLength * 0.5);
    };

    viewX = ensureMinLength(viewX, minXLength);
    viewY = ensureMinLength(viewY, minYLength);
    viewX = constrainToContent(viewX, contentX);
    viewY = constrainToContent(viewY, contentY);
}

juce::Range<double> ViewportController::paddedRange(const juce::Range<double>& range, double paddingFactor) const
{
    const double length = range.getLength();
    if (length <= 0.0)
        return { range.getStart() - 0.5, range.getStart() + 0.5 };

    return range.expanded(length * paddingFactor);
}

juce::Range<double> ViewportController::constrainToContent(const juce::Range<double>& view,
                                                           const juce::Range<double>& content) const
{
    if (view.getLength() >= content.getLength())
    {
        const double centre = content.getStart() + content.getLength() * 0.5;
        return { centre - view.getLength() * 0.5, centre + view.getLength() * 0.5 };
    }

    if (view.getStart() < content.getStart())
        return { content.getStart(), content.getStart() + view.getLength() };
    if (view.getEnd() > content.getEnd())
        return { content.getEnd() - view.getLength(), content.getEnd() };

    return view;
}

juce::Range<double> ViewportController::zoomRange(const juce::Range<double>& view, double factor) const
{
    const double centre = view.getStart() + view.getLength() * 0.5;
    const double length = view.getLength() * factor;
    return { centre - length * 0.5, centre + length * 0.5 };
}
