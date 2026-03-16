#pragma once

#include "ServerCommand.h"

class StopCommand : public ServerCommand
{
public:
	wstring getName() { return L"stop"; }
	wstring getUsage() { return L"/stop"; }
	void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server);
};
