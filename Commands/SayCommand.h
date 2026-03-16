#pragma once
#include "ServerCommand.h"

class SayCommand : public ServerCommand
{
public:
	wstring getName() { return L"say"; }
	wstring getUsage() { return L"/say <message ...> - Broadcasts a message to all players"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
