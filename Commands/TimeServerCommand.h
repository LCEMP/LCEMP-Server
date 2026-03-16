#pragma once

#include "ServerCommand.h"

class TimeServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"time"; }
	wstring getUsage() { return L"/time <set|add> <value>"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
