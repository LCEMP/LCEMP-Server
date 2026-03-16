#pragma once
#include "ServerCommand.h"

class BanIpCommand : public ServerCommand
{
public:
	wstring getName() { return L"ban-ip"; }
	wstring getUsage() { return L"/ban-ip <address|name> - Bans an IP address"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
