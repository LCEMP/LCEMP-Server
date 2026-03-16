#include "stdafx.h"
#include "WhitelistCommand.h"
#include "ServerCommand.h"
#include "ServerTextList.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../Core/ServerLists.h"
#include "ServerTextList.h"

#include <sstream>
#include <cwctype>

void WhitelistCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring subCmd;
	if (!(ss >> subCmd))
	{
		src->warn(L"Usage: /whitelist <on|off|list|add|remove|reload>");
		return;
	}

	if (subCmd == L"on")
	{
		ServerLists_SetWhitelistEnabled(true);
		ServerCommand::notifyAdmins(src, server, L"Turned on the whitelist");
	}
	else if (subCmd == L"off")
	{
		ServerLists_SetWhitelistEnabled(false);
		ServerCommand::notifyAdmins(src, server, L"Turned off the whitelist");
	}
	else if (subCmd == L"list")
	{
		const set<wstring>& entries = ServerLists_GetWhitelist()->getEntries();
		wchar_t buf[128];
		swprintf_s(buf, 128, L"There are %d whitelisted players:", (int)entries.size());
		src->info(wstring(buf));
		wstring list;
		for (set<wstring>::const_iterator it = entries.begin(); it != entries.end(); ++it)
		{
			if (!list.empty()) list += L", ";
			list += *it;
		}
		if (!list.empty()) src->info(list);
	}
	else if (subCmd == L"add")
	{
		wstring playerName;
		if (!(ss >> playerName))
		{
			src->warn(L"Usage: /whitelist add <player>");
			return;
		}
		wstring lower = playerName;
		for (size_t i = 0; i < lower.size(); i++)
			lower[i] = towlower(lower[i]);
		ServerLists_GetWhitelist()->add(lower);
		ServerCommand::notifyAdmins(src, server, L"Added " + playerName + L" to the whitelist");
	}
	else if (subCmd == L"remove")
	{
		wstring playerName;
		if (!(ss >> playerName))
		{
			src->warn(L"Usage: /whitelist remove <player>");
			return;
		}
		wstring lower = playerName;
		for (size_t i = 0; i < lower.size(); i++)
			lower[i] = towlower(lower[i]);
		ServerLists_GetWhitelist()->remove(lower);
		ServerCommand::notifyAdmins(src, server, L"Removed " + playerName + L" from the whitelist");
	}
	else if (subCmd == L"reload")
	{
		ServerLists_GetWhitelist()->load();
		ServerCommand::notifyAdmins(src, server, L"Reloaded the whitelist");
	}
	else
	{
		src->warn(L"Usage: /whitelist <on|off|list|add|remove|reload>");
	}
}
