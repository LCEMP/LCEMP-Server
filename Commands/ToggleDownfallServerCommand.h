#pragma once

#include "ServerCommand.h"

class ToggleDownfallServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"toggledownfall"; }
	wstring getUsage() { return L"/toggledownfall"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
