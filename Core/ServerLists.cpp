#include "stdafx.h"
#include "ServerLists.h"
#include "ServerProperties.h"
#include "../Commands/ServerTextList.h"
#include <cwctype>

static ServerTextList* s_bannedPlayers = NULL;
static ServerTextList* s_bannedIPs = NULL;
static ServerTextList* s_whitelist = NULL;
static ServerTextList* s_ops = NULL;
static bool s_whitelistEnabled = false;
static ServerProperties* s_properties = NULL;

void ServerLists_Init(bool whitelistEnabled, ServerProperties* props)
{
	static ServerTextList bannedPlayers(L"banned-players.txt");
	static ServerTextList bannedIPs(L"banned-ips.txt");
	static ServerTextList whitelist(L"white-list.txt");
	static ServerTextList ops(L"ops.txt");

	s_bannedPlayers = &bannedPlayers;
	s_bannedIPs = &bannedIPs;
	s_whitelist = &whitelist;
	s_ops = &ops;
	s_whitelistEnabled = whitelistEnabled;
	s_properties = props;
}

ServerTextList* ServerLists_GetBannedPlayers() { return s_bannedPlayers; }
ServerTextList* ServerLists_GetBannedIPs() { return s_bannedIPs; }
ServerTextList* ServerLists_GetWhitelist() { return s_whitelist; }
ServerTextList* ServerLists_GetOps() { return s_ops; }

bool ServerLists_IsWhitelistEnabled() { return s_whitelistEnabled; }
void ServerLists_SetWhitelistEnabled(bool enabled)
{
	s_whitelistEnabled = enabled;
	if (s_properties)
	{
		s_properties->whiteList = enabled;
		s_properties->save(L"server.properties");
	}
}

bool ServerLists_IsPlayerBanned(const std::wstring& name)
{
	if (!s_bannedPlayers) return false;
	return s_bannedPlayers->contains(name);
}

bool ServerLists_IsIPBanned(const std::wstring& ip)
{
	if (!s_bannedIPs) return false;
	return s_bannedIPs->contains(ip);
}

bool ServerLists_IsPlayerOp(const std::wstring& name)
{
	if (!s_ops) return false;
	return s_ops->contains(name);
}

bool ServerLists_IsPlayerWhitelisted(const std::wstring& name)
{
	if (!s_whitelistEnabled) return true;
	if (ServerLists_IsPlayerOp(name)) return true;
	if (!s_whitelist) return true;

	std::wstring lower = name;
	for (size_t i = 0; i < lower.size(); i++)
		lower[i] = towlower(lower[i]);
	return s_whitelist->contains(lower);
}
