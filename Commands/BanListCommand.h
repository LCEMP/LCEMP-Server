#pragma once
#include "ServerCommand.h"

class BanListCommand : public ServerCommand
{
public:
	wstring getName() { return L"banlist"; }
	wstring getUsage() { return L"/banlist [ips|players] - Shows the ban list"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
