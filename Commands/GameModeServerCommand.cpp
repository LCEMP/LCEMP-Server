#include "stdafx.h"
#include "GameModeServerCommand.h"
#include "ServerCommand.h"
#include "ConsoleCommandSender.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.World/LevelSettings.h"

#include <sstream>

void GameModeServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring modeStr;

	if (!(ss >> modeStr))
	{
		src->warn(L"\u00A7cUsage: /gamemode <mode> [player]");
		return;
	}

	GameType *mode = NULL;
	if (modeStr == L"0" || modeStr == L"s" || modeStr == L"survival")
		mode = GameType::SURVIVAL;
	else if (modeStr == L"1" || modeStr == L"c" || modeStr == L"creative")
		mode = GameType::CREATIVE;
	else if (modeStr == L"2" || modeStr == L"a" || modeStr == L"adventure")
		mode = GameType::ADVENTURE;
	else
	{
		src->warn(L"\u00A7cUnknown game mode: " + modeStr);
		return;
	}

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
	player->setGameMode(mode);
	wchar_t buf[256];
	swprintf_s(buf, 256, L"Set %s's game mode to %s", player->name.c_str(), mode->getName().c_str());
	ServerCommand::notifyAdmins(src, server, wstring(buf));
}
