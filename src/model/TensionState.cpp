#include "TensionState.h"

const juce::Identifier TensionState::typeId{"Tension"};
const juce::Identifier TensionState::pointsId{"points"};

bool TensionState::Blend::operator==(const Blend& other) const
{
    return harmonic == other.harmonic && noise == other.noise;
}

bool TensionState::Blend::operator!=(const Blend& other) const
{
    return ! (*this == other);
}

TensionState::TensionState(juce::ValueTree tree)
    : state(std::move(tree))
{
}

TensionState TensionState::create()
{
    juce::ValueTree tree(typeId);
    return TensionState(tree);
}

bool TensionState::isTension(const juce::ValueTree& tree)
{
    return tree.hasType(typeId);
}

TensionState::Blend TensionState::blendFromTension(double tension)
{
    const double harmonic = juce::jlimit(0.0, 1.0, tension);
    return { harmonic, 1.0 - harmonic };
}

bool TensionState::isValid() const
{
    return state.isValid() && state.hasType(typeId);
}

juce::ValueTree TensionState::getValueTree() const
{
    return state;
}

juce::var TensionState::getPoints() const
{
    return state.getProperty(pointsId);
}

juce::Array<double> TensionState::getPointValues() const
{
    juce::Array<double> result;
    if (const auto* array = getPoints().getArray())
    {
        result.ensureStorageAllocated(array->size());
        for (const auto& value : *array)
            result.add(juce::jlimit(0.0, 1.0, static_cast<double>(value)));
    }
    return result;
}

TensionState::Blend TensionState::getBlendForIndex(int index, double fallback) const
{
    const auto values = getPointValues();
    if (values.isEmpty())
        return blendFromTension(fallback);

    const int safeIndex = juce::jlimit(0, values.size() - 1, index);
    return blendFromTension(values[safeIndex]);
}

void TensionState::setPoints(const juce::var& points, juce::UndoManager* undoManager)
{
    if (const auto* array = points.getArray())
    {
        juce::Array<juce::var> sanitized;
        sanitized.ensureStorageAllocated(array->size());
        for (const auto& value : *array)
            sanitized.add(juce::jlimit(0.0, 1.0, static_cast<double>(value)));
        state.setProperty(pointsId, sanitized, undoManager);
    }
    else
    {
        state.setProperty(pointsId, points, undoManager);
    }
}

void TensionState::addListener(juce::ValueTree::Listener* listener)
{
    state.addListener(listener);
}

void TensionState::removeListener(juce::ValueTree::Listener* listener)
{
    state.removeListener(listener);
}
