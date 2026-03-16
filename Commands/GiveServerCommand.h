#pragma once

#include "ServerCommand.h"

class GiveServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"give"; }
	wstring getUsage() { return L"/give <player> <item id> [amount] [data]"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
