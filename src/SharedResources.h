#pragma once

#include <juce_core/juce_core.h>

class SharedResources
{
public:
    static juce::MemoryBlock getEmbeddedResource(const char* resourceName);
};
