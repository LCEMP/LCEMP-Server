#pragma once
#include "ServerCommand.h"

class WhitelistCommand : public ServerCommand
{
public:
	wstring getName() { return L"whitelist"; }
	wstring getUsage() { return L"/whitelist <on|off|list|add|remove|reload> [player] - Manages the whitelist"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
