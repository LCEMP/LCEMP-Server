#include "stdafx.h"
#include "KickServerCommand.h"
#include "ServerCommand.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.Client/PlayerConnection.h"
#include "../../Minecraft.World/DisconnectPacket.h"

#include <sstream>

void KickServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;

	if (!(ss >> playerName))
	{
		src->warn(L"\u00A7cUsage: /kick <player> [reason ...]");
		return;
	}

	PlayerList *players = server->getPlayers();
	shared_ptr<ServerPlayer> player = players->getPlayer(playerName);
	if (!player)
	{
		src->warn(L"\u00A7cThat player cannot be found");
		return;
	}

	wstring reason;
	getline(ss, reason);
	size_t rstart = reason.find_first_not_of(L" \t");
	if (rstart != wstring::npos) reason = reason.substr(rstart);
	else reason.clear();

	player->connection->disconnect(DisconnectPacket::eDisconnect_Kicked);

	if (!reason.empty())
		ServerCommand::notifyAdmins(src, server, L"Kicked player " + player->name + L": " + reason);
	else
		ServerCommand::notifyAdmins(src, server, L"Kicked player " + player->name);
}
