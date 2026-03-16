#pragma once

#include "ServerCommand.h"

class KickServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"kick"; }
	wstring getUsage() { return L"/kick <player> [reason ...]"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
