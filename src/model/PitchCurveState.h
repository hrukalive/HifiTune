#pragma once

#include <juce_data_structures/juce_data_structures.h>

class PitchCurveState
{
public:
    explicit PitchCurveState(juce::ValueTree tree);

    static PitchCurveState create();
    static bool isPitchCurve(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    juce::var getPoints() const;
    void setPoints(const juce::var& points, juce::UndoManager* undoManager);

    juce::var getConfidence() const;
    void setConfidence(const juce::var& confidence, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier pointsId;
    static const juce::Identifier confidenceId;

private:
    juce::ValueTree state;
};
