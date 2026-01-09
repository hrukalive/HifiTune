#include "CommandManager.h"

CommandManager::CommandManager()
{
    commandManager.setFirstCommandTarget(nullptr);
}

juce::ApplicationCommandManager& CommandManager::getManager()
{
    return commandManager;
}

const juce::ApplicationCommandManager& CommandManager::getManager() const
{
    return commandManager;
}

void CommandManager::registerTarget(juce::ApplicationCommandTarget* target)
{
    commandManager.registerAllCommandsForTarget(target);
    commandManager.setFirstCommandTarget(target);
}

juce::KeyPressMappingSet& CommandManager::getKeyMappings()
{
    return commandManager.getKeyMappings();
}
