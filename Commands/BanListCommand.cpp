#include "stdafx.h"
#include "BanListCommand.h"
#include "ServerTextList.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>

void BanListCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring type;
	ss >> type;

	if (type == L"ips")
	{
		const set<wstring>& entries = ServerLists_GetBannedIPs()->getEntries();
		wchar_t buf[128];
		swprintf_s(buf, 128, L"There are %d banned IP addresses:", (int)entries.size());
		src->info(wstring(buf));
		wstring list;
		for (set<wstring>::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			if (!list.empty()) list += L", ";
			list += *it;
		}
		if (!list.empty()) src->info(list);
	}
	else
	{
		const set<wstring>& entries = ServerLists_GetBannedPlayers()->getEntries();
		wchar_t buf[128];
		swprintf_s(buf, 128, L"There are %d banned players:", (int)entries.size());
		src->info(wstring(buf));
		wstring list;
		for (set<wstring>::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			if (!list.empty()) list += L", ";
			list += *it;
		}
		if (!list.empty()) src->info(list);
	}
}
