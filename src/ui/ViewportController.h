#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ViewportController
{
public:
    void setContentRanges(juce::Range<double> xRange, juce::Range<double> yRange);
    void setMinimumViewSize(double minX, double minY);
    void fitToContent(double paddingFactor = 0.1);
    bool handleWheel(const juce::MouseWheelDetails& details, const juce::ModifierKeys& modifiers);

    const juce::Range<double>& getViewX() const { return viewX; }
    const juce::Range<double>& getViewY() const { return viewY; }

private:
    void clampView();
    juce::Range<double> paddedRange(const juce::Range<double>& range, double paddingFactor) const;
    juce::Range<double> constrainToContent(const juce::Range<double>& view, const juce::Range<double>& content) const;
    juce::Range<double> zoomRange(const juce::Range<double>& view, double factor) const;

    juce::Range<double> contentX { 0.0, 1.0 };
    juce::Range<double> contentY { 0.0, 1.0 };
    juce::Range<double> viewX { 0.0, 1.0 };
    juce::Range<double> viewY { 0.0, 1.0 };
    double minViewX = 0.05;
    double minViewY = 0.05;
};
