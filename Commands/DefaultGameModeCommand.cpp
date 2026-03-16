#include "stdafx.h"
#include "DefaultGameModeCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.World/LevelSettings.h"

#include <sstream>

void DefaultGameModeCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring modeStr;
	if (!(ss >> modeStr))
	{
		src->warn(L"Usage: /defaultgamemode <mode>");
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
		src->warn(L"Unknown game mode: " + modeStr);
		return;
	}

	server->getPlayers()->setOverrideGameMode(mode);
	ServerCommand::notifyAdmins(src, server, L"The world's default game mode is now " + mode->getName());
}
