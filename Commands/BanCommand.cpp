#include "stdafx.h"
#include "BanCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.Client/PlayerConnection.h"
#include "../../Minecraft.World/DisconnectPacket.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>

void BanCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;
	if (!(ss >> playerName))
	{
		src->warn(L"Usage: /ban <player>");
		return;
	}

	ServerLists_GetBannedPlayers()->add(playerName);
	ServerCommand::notifyAdmins(src, server, L"Banned player " + playerName);

	shared_ptr<ServerPlayer> player = server->getPlayers()->getPlayer(playerName);
	if (player)
		player->connection->disconnect(DisconnectPacket::eDisconnect_Banned);
}
