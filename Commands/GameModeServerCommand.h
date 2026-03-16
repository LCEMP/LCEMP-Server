#pragma once

#include "ServerCommand.h"

class GameModeServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"gamemode"; }
	wstring getUsage() { return L"/gamemode <mode> [player]"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
