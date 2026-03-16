#pragma once
#include "ServerCommand.h"

class DeOpCommand : public ServerCommand
{
public:
	wstring getName() { return L"deop"; }
	wstring getUsage() { return L"/deop <player> - Removes operator status from a player"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
