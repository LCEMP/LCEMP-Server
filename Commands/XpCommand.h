#pragma once
#include "ServerCommand.h"

class XpCommand : public ServerCommand
{
public:
	wstring getName() { return L"xp"; }
	wstring getUsage() { return L"/xp <amount> [player] OR /xp <amount>L [player] - Gives experience"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
