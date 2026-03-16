#pragma once

#include "ServerCommand.h"

class ConsoleCommandDispatcher;

class HelpCommand : public ServerCommand
{
private:
	ConsoleCommandDispatcher *dispatcher;

public:
	HelpCommand(ConsoleCommandDispatcher *dispatcher);

	wstring getName() { return L"help"; }
	wstring getUsage() { return L"/help [page|command name]"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
