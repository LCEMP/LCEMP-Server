#include "stdafx.h"
#include "PardonCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>

void PardonCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;
	if (!(ss >> playerName))
	{
		src->warn(L"Usage: /pardon <player>");
		return;
	}

	ServerLists_GetBannedPlayers()->remove(playerName);
	ServerCommand::notifyAdmins(src, server, L"Unbanned player " + playerName);
}
