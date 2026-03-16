#include "stdafx.h"
#include "KillServerCommand.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.World/DamageSource.h"

#include <sstream>

void KillServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;

	if (!(ss >> playerName))
	{
		src->warn(L"\u00A7cYou must specify which player you wish to perform this action on.");
		return;
	}

	PlayerList *players = server->getPlayers();
	shared_ptr<ServerPlayer> player = players->getPlayer(playerName);
	if (!player)
	{
		src->warn(L"\u00A7cThat player cannot be found");
		return;
	}

	player->hurt(DamageSource::outOfWorld, 1000);
	src->info(L"Ouch. That look like it hurt.");
}
