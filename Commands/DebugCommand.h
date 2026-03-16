#pragma once
#include "ServerCommand.h"

class DebugCommand : public ServerCommand
{
public:
	wstring getName() { return L"debug"; }
	wstring getUsage() { return L"/debug <start|stop> - Starts or stops a debugging session"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
