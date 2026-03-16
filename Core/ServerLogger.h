#pragma once

#include <string>

void ServerLog_Init();
void ServerLog_Close();
void ServerLog(const wchar_t* fmt, ...);
void ServerWarn(const wchar_t* fmt, ...);
void ServerLog_RegisterInput(std::wstring* buffer);
void ServerLog_UnregisterInput();
void ServerLog_LockOutput();
void ServerLog_UnlockOutput();
