#include "stdafx.h"
#include "SaveAllCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerLevel.h"

void SaveAllCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	ServerCommand::notifyAdmins(src, server, L"Forcing save..");
	PlayerList *players = server->getPlayers();
	if (players)
		players->saveAll(NULL);
	for (unsigned int i = 0; i < server->levels.length; i++)
	{
		ServerLevel *level = server->levels[i];
		if (level)
			level->save(true, NULL);
	}
	ServerCommand::notifyAdmins(src, server, L"Save complete.");
}
