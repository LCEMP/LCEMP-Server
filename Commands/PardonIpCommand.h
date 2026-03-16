#pragma once
#include "ServerCommand.h"

class PardonIpCommand : public ServerCommand
{
public:
	wstring getName() { return L"pardon-ip"; }
	wstring getUsage() { return L"/pardon-ip <ip> - Unbans an IP address"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
