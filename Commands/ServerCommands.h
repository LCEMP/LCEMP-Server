#pragma once

#include <vector>

class MinecraftServer;
class ConsoleInputSource;
class ConsoleCommandDispatcher;

ConsoleCommandDispatcher *CreateConsoleCommandDispatcher();
bool HandleServerCommand(const wstring& cmd, ConsoleInputSource *src, MinecraftServer *server);
vector<wstring> GetServerCommandCompletions(const wstring& input, MinecraftServer *server);