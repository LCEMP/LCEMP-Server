#pragma once

#include "ServerCommand.h"

class ListServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"list"; }
	wstring getUsage() { return L"/list"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
