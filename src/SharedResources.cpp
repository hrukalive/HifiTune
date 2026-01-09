#include "SharedResources.h"
#include "BinaryData.h"

juce::MemoryBlock SharedResources::getEmbeddedResource(const char* resourceName)
{
    int dataSize = 0;
    if (const void* data = BinaryData::getNamedResource(resourceName, dataSize))
    {
        return juce::MemoryBlock(data, static_cast<size_t>(dataSize));
    }

    return {};
}
