#include "stdafx.h"
#include "StopCommand.h"
#include "ServerCommand.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"

void StopCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	ServerCommand::notifyAdmins(src, server, L"Stopping the server");
	MinecraftServer::HaltServer();
}
