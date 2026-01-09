#pragma once

#include <juce_data_structures/juce_data_structures.h>

class TensionState
{
public:
    struct Blend
    {
        double harmonic{0.5};
        double noise{0.5};

        bool operator==(const Blend& other) const;
        bool operator!=(const Blend& other) const;
    };

    explicit TensionState(juce::ValueTree tree);

    static TensionState create();
    static bool isTension(const juce::ValueTree& tree);
    static Blend blendFromTension(double tension);

    bool isValid() const;
    juce::ValueTree getValueTree() const;

    juce::var getPoints() const;
    juce::Array<double> getPointValues() const;
    Blend getBlendForIndex(int index, double fallback = 0.5) const;
    void setPoints(const juce::var& points, juce::UndoManager* undoManager);

    void addListener(juce::ValueTree::Listener* listener);
    void removeListener(juce::ValueTree::Listener* listener);

    static const juce::Identifier typeId;
    static const juce::Identifier pointsId;

private:
    juce::ValueTree state;
};
