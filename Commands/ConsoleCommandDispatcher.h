#pragma once

#include "ServerCommand.h"
#include <map>

class ConsoleCommandDispatcher
{
private:
	map<wstring, ServerCommand *> commandsByName;

public:
	~ConsoleCommandDispatcher();

	void addCommand(ServerCommand *command);
	bool performCommand(const wstring& name, const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
	const map<wstring, ServerCommand *>& getCommands() const;
};
