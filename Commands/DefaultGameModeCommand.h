#pragma once
#include "ServerCommand.h"

class DefaultGameModeCommand : public ServerCommand
{
public:
	wstring getName() { return L"defaultgamemode"; }
	wstring getUsage() { return L"/defaultgamemode <mode> - Sets the default game mode"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
