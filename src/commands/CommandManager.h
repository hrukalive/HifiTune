#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CommandManager final
{
public:
    CommandManager();

    juce::ApplicationCommandManager& getManager();
    const juce::ApplicationCommandManager& getManager() const;

    void registerTarget(juce::ApplicationCommandTarget* target);
    juce::KeyPressMappingSet& getKeyMappings();

private:
    juce::ApplicationCommandManager commandManager;
};
