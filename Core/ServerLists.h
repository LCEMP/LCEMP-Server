#pragma once
#include <string>

class ServerTextList;
class ServerProperties;

void ServerLists_Init(bool whitelistEnabled, ServerProperties* props = 0);

ServerTextList* ServerLists_GetBannedPlayers();
ServerTextList* ServerLists_GetBannedIPs();
ServerTextList* ServerLists_GetWhitelist();
ServerTextList* ServerLists_GetOps();

bool ServerLists_IsWhitelistEnabled();
void ServerLists_SetWhitelistEnabled(bool enabled);

bool ServerLists_IsPlayerBanned(const std::wstring& name);
bool ServerLists_IsPlayerWhitelisted(const std::wstring& name);
bool ServerLists_IsPlayerOp(const std::wstring& name);
bool ServerLists_IsIPBanned(const std::wstring& ip);
