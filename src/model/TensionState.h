#pragma once

#include <juce_data_structures/juce_data_structures.h>

class TensionState
{
public:
    explicit TensionState(juce::ValueTree tree);

    static TensionState create();
    static bool isTension(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    juce::var getPoints() const;
    void setPoints(const juce::var& points, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier pointsId;

private:
    juce::ValueTree state;
};
