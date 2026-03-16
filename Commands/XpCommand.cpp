#include "stdafx.h"
#include "XpCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"

#include <sstream>

void XpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring amountStr;
	if (!(ss >> amountStr))
	{
		src->warn(L"Usage: /xp <amount> [player] OR /xp <amount>L [player]");
		return;
	}

	bool isLevels = false;
	if (!amountStr.empty() && (amountStr.back() == L'l' || amountStr.back() == L'L'))
	{
		isLevels = true;
		amountStr = amountStr.substr(0, amountStr.size() - 1);
	}

	int amount = _wtoi(amountStr.c_str());

	wstring playerName;
	if (!(ss >> playerName))
	{
		src->warn(L"Usage: /xp <amount> <player>");
		return;
	}

	PlayerList *players = server->getPlayers();
	shared_ptr<ServerPlayer> player = players->getPlayer(playerName);
	if (!player)
	{
		src->warn(L"Player not found: " + playerName);
		return;
	}

	wchar_t buf[256];
	if (isLevels)
	{
		player->withdrawExperienceLevels(-amount);
		swprintf_s(buf, 256, L"Given %d experience levels to %s", amount, player->name.c_str());
	}
	else
	{
		if (amount < 0)
		{
			src->warn(L"Cannot give negative experience points");
			return;
		}
		player->increaseXp(amount);
		swprintf_s(buf, 256, L"Given %d experience to %s", amount, player->name.c_str());
	}
	ServerCommand::notifyAdmins(src, server, wstring(buf));
}
