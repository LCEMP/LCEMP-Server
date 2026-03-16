#pragma once
#include "ServerCommand.h"

class MeCommand : public ServerCommand
{
public:
	wstring getName() { return L"me"; }
	wstring getUsage() { return L"/me <action ...> - Performs the specified action"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
