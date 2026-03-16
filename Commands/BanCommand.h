#pragma once
#include "ServerCommand.h"

class BanCommand : public ServerCommand
{
public:
	wstring getName() { return L"ban"; }
	wstring getUsage() { return L"/ban <player> - Bans a player from the server"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
