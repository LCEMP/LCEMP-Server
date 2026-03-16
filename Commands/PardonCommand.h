#pragma once
#include "ServerCommand.h"

class PardonCommand : public ServerCommand
{
public:
	wstring getName() { return L"pardon"; }
	wstring getUsage() { return L"/pardon <player> - Unbans a player"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
