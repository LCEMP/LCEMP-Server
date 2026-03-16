#pragma once
#include "ServerCommand.h"

class SaveOffCommand : public ServerCommand
{
public:
	wstring getName() { return L"save-off"; }
	wstring getUsage() { return L"/save-off - Disables automatic server saves"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
