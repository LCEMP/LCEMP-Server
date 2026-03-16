#pragma once
#include "ServerCommand.h"

class SeedCommand : public ServerCommand
{
public:
	wstring getName() { return L"seed"; }
	wstring getUsage() { return L"/seed - Displays the world seed"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
