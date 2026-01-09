#pragma once

#include <juce_data_structures/juce_data_structures.h>

class ClipState
{
public:
    explicit ClipState(juce::ValueTree tree);

    static ClipState create(double startBeat, double lengthBeats);
    static bool isClip(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    double getStartBeat() const;
    double getLengthBeats() const;

    void setStartBeat(double startBeat, juce::UndoManager* undoManager);
    void setLengthBeats(double lengthBeats, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier startBeatId;
    static const juce::Identifier lengthBeatsId;

private:
    juce::ValueTree state;
};
