#include "stdafx.h"
#include "ConsoleCommandDispatcher.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"

ConsoleCommandDispatcher::~ConsoleCommandDispatcher()
{
	for (AUTO_VAR(it, commandsByName.begin()); it != commandsByName.end(); ++it)
		delete it->second;
	commandsByName.clear();
}

void ConsoleCommandDispatcher::addCommand(ServerCommand *command)
{
	commandsByName[command->getName()] = command;
}

bool ConsoleCommandDispatcher::performCommand(const wstring& name, const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	AUTO_VAR(it, commandsByName.find(name));
	if (it != commandsByName.end())
	{
		it->second->execute(args, src, server);
		return true;
	}
	return false;
}

const map<wstring, ServerCommand *>& ConsoleCommandDispatcher::getCommands() const
{
	return commandsByName;
}
