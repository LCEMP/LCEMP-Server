#include "stdafx.h"
#include "DeOpCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.World/net.minecraft.network.packet.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

void DeOpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstring name = args;
	size_t start = name.find_first_not_of(L" \t");
	if (start == wstring::npos || name.empty())
	{
		src->warn(L"Usage: /deop <player>");
		return;
	}
	name = name.substr(start);
	size_t end = name.find_first_of(L" \t");
	if (end != wstring::npos)
		name = name.substr(0, end);

	ServerLists_GetOps()->remove(name);

	shared_ptr<ServerPlayer> player = server->getPlayers()->getPlayer(name);
	if (player != nullptr)
	{
		player->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_Op, 0);
		server->getPlayers()->broadcastAll(shared_ptr<PlayerInfoPacket>(new PlayerInfoPacket(player)));
	}

	ServerCommand::notifyAdmins(src, server, L"De-opped " + name);
}
