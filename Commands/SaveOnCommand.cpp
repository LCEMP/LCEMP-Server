#include "stdafx.h"
#include "SaveOnCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/ServerLevel.h"

void SaveOnCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	for (unsigned int i = 0; i < server->levels.length; i++)
	{
		ServerLevel *level = server->levels[i];
		if (level)
			level->noSave = false;
	}
	ServerCommand::notifyAdmins(src, server, L"Turned on world auto-saving");
}
