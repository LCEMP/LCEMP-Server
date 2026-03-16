#include "stdafx.h"
#include "ListServerCommand.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"

void ListServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	PlayerList *playerList = server->getPlayers();
	int count = playerList->getPlayerCount();
	int max = playerList->getMaxPlayers();

	wchar_t buf[128];
	swprintf_s(buf, 128, L"There are %d/%d players online:", count, max);
	src->info(wstring(buf));
	src->info(playerList->getPlayerNames());
}
