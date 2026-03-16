#include "stdafx.h"
#include "SaveOffCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/ServerLevel.h"

void SaveOffCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	for (unsigned int i = 0; i < server->levels.length; i++)
	{
		ServerLevel *level = server->levels[i];
		if (level)
			level->noSave = true;
	}
	ServerCommand::notifyAdmins(src, server, L"Turned off world auto-saving");
}
