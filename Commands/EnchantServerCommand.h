#pragma once

#include "ServerCommand.h"

class EnchantServerCommand : public ServerCommand
{
public:
	wstring getName() { return L"enchant"; }
	wstring getUsage() { return L"/enchant <player> <enchantment ID> [level]"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
