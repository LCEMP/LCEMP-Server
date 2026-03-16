#include "stdafx.h"
#include "BanIpCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.Client/PlayerConnection.h"
#include "../../Minecraft.World/DisconnectPacket.h"
#include "../../Minecraft.World/Connection.h"
#include "../../Minecraft.World/Socket.h"
#include "../../Minecraft.Client/Windows64/Network/WinsockNetLayer.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>

static bool isValidIP(const wstring& str)
{
	int dots = 0;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == L'.') dots++;
		else if (str[i] < L'0' || str[i] > L'9') return false;
	}
	return dots == 3 && str.size() >= 7;
}

static wstring narrowToWide(const std::string& s)
{
	return wstring(s.begin(), s.end());
}

void BanIpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring arg;
	if (!(ss >> arg))
	{
		src->warn(L"Usage: " + getUsage());
		return;
	}

	wstring ip;
	if (isValidIP(arg))
	{
		ip = arg;
	}
	else
	{
		if (!server || !server->getPlayers())
		{
			src->warn(L"Cannot resolve player name, no players online");
			return;
		}
		shared_ptr<ServerPlayer> target = server->getPlayers()->getPlayer(arg);
		if (!target || !target->connection)
		{
			src->warn(L"Cannot find player \"" + arg + L"\"");
			return;
		}
		Connection *conn = target->connection->connection;
		if (!conn || !conn->getSocket())
		{
			src->warn(L"Cannot determine IP for \"" + arg + L"\"");
			return;
		}
		BYTE smallId = conn->getSocket()->getSmallId();
		std::string ipStr = WinsockNetLayer::GetIPForSmallId(smallId);
		if (ipStr.empty())
		{
			src->warn(L"Cannot determine IP for \"" + arg + L"\"");
			return;
		}
		ip = narrowToWide(ipStr);
	}

	ServerLists_GetBannedIPs()->add(ip);
	ServerCommand::notifyAdmins(src, server, L"Banned IP address " + ip);

	if (server && server->getPlayers())
	{
		vector<shared_ptr<ServerPlayer>>& pl = server->getPlayers()->players;
		for (unsigned int i = 0; i < pl.size(); i++)
		{
			if (!pl[i] || !pl[i]->connection) continue;
			Connection *conn = pl[i]->connection->connection;
			if (!conn || !conn->getSocket()) continue;
			BYTE sid = conn->getSocket()->getSmallId();
			std::string pIp = WinsockNetLayer::GetIPForSmallId(sid);
			if (narrowToWide(pIp) == ip)
			{
				server->getPlayers()->kickPlayerByShortId(sid);
			}
		}
	}
}
