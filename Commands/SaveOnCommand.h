#pragma once
#include "ServerCommand.h"

class SaveOnCommand : public ServerCommand
{
public:
	wstring getName() { return L"save-on"; }
	wstring getUsage() { return L"/save-on - Enables automatic server saves"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
