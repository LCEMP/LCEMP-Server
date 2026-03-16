#pragma once

#include "ServerCommand.h"

class TpCommand : public ServerCommand
{
public:
	wstring getName() { return L"tp"; }
	wstring getUsage() { return L"/tp [player] <x> <y> <z> OR /tp [player] <player>"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
