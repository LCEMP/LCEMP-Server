#pragma once
#include "ServerCommand.h"

class OpCommand : public ServerCommand
{
public:
	wstring getName() { return L"op"; }
	wstring getUsage() { return L"/op <player> - Grants operator status to a player"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
