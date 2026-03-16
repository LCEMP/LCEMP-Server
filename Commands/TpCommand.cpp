#include "stdafx.h"
#include "TpCommand.h"
#include "ServerCommand.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.Client/PlayerConnection.h"

#include <sstream>
#include <vector>

void TpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	vector<wstring> tokens;
	wstring token;
	while (ss >> token)
		tokens.push_back(token);

	PlayerList *players = server->getPlayers();

	if (tokens.size() == 2)
	{
		shared_ptr<ServerPlayer> subject = players->getPlayer(tokens[0]);
		if (!subject)
		{
			src->warn(L"\u00A7cThat player cannot be found");
			return;
		}
		shared_ptr<ServerPlayer> dest = players->getPlayer(tokens[1]);
		if (!dest)
		{
			src->warn(L"\u00A7cThat player cannot be found");
			return;
		}
		subject->moveTo(dest->x, dest->y, dest->z, dest->yRot, dest->xRot);
		if (subject->connection)
			subject->connection->teleport(dest->x, dest->y, dest->z, dest->yRot, dest->xRot);
		wchar_t buf[256];
		swprintf_s(buf, 256, L"Teleported %s to %s", subject->name.c_str(), dest->name.c_str());
		ServerCommand::notifyAdmins(src, server, wstring(buf));
	}
	else if (tokens.size() == 4)
	{
		shared_ptr<ServerPlayer> subject = players->getPlayer(tokens[0]);
		if (!subject)
		{
			src->warn(L"\u00A7cThat player cannot be found");
			return;
		}
		int x = _wtoi(tokens[1].c_str());
		int y = _wtoi(tokens[2].c_str());
		int z = _wtoi(tokens[3].c_str());
		subject->moveTo(x + 0.5, (double)y, z + 0.5, subject->yRot, subject->xRot);
		if (subject->connection)
			subject->connection->teleport(x + 0.5, (double)y, z + 0.5, subject->yRot, subject->xRot);
		wchar_t buf[256];
		swprintf_s(buf, 256, L"Teleported %s to %d, %d, %d", subject->name.c_str(), x, y, z);
		ServerCommand::notifyAdmins(src, server, wstring(buf));
	}
	else
	{
		src->warn(L"\u00A7cUsage: /tp [player] <x> <y> <z> OR /tp [player] <player>");
	}
}
