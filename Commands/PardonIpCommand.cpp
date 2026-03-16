#include "stdafx.h"
#include "PardonIpCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>

void PardonIpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring ip;
	if (!(ss >> ip))
	{
		src->warn(L"Usage: /pardon-ip <ip>");
		return;
	}

	ServerLists_GetBannedIPs()->remove(ip);
	ServerCommand::notifyAdmins(src, server, L"Unbanned IP address " + ip);
}
