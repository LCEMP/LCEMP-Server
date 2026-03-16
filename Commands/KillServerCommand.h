#pragma once

#include "ServerCommand.h"

class KillServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"kill"; }
	wstring getUsage() { return L"/kill <player>"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
