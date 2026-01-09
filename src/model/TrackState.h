#pragma once

#include <juce_data_structures/juce_data_structures.h>

class TrackState
{
public:
    explicit TrackState(juce::ValueTree tree);

    static TrackState create(const juce::String& name);
    static bool isTrack(const juce::ValueTree& tree);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    juce::String getName() const;
    void setName(const juce::String& name, juce::UndoManager* undoManager);

    juce::var getColour() const;
    void setColour(const juce::var& colour, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier nameId;
    static const juce::Identifier colourId;

private:
    juce::ValueTree state;
};
