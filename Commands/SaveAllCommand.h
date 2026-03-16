#pragma once
#include "ServerCommand.h"

class SaveAllCommand : public ServerCommand
{
public:
	wstring getName() { return L"save-all"; }
	wstring getUsage() { return L"/save-all - Forces the server to save"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
