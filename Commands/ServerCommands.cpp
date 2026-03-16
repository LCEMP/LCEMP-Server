#include "stdafx.h"
#include "ServerCommands.h"
#include "ConsoleCommandDispatcher.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ServerPlayer.h"

#include "StopCommand.h"
#include "HelpCommand.h"
#include "TpCommand.h"
#include "TimeServerCommand.h"
#include "ToggleDownfallServerCommand.h"
#include "GiveServerCommand.h"
#include "EnchantServerCommand.h"
#include "KillServerCommand.h"
#include "GameModeServerCommand.h"
#include "ListServerCommand.h"
#include "KickServerCommand.h"
#include "SayCommand.h"
#include "MeCommand.h"
#include "SeedCommand.h"
#include "XpCommand.h"
#include "DefaultGameModeCommand.h"
#include "SaveAllCommand.h"
#include "SaveOffCommand.h"
#include "SaveOnCommand.h"
#include "DebugCommand.h"
#include "OpCommand.h"
#include "DeOpCommand.h"
#include "BanCommand.h"
#include "PardonCommand.h"
#include "BanIpCommand.h"
#include "PardonIpCommand.h"
#include "BanListCommand.h"
#include "WhitelistCommand.h"

#include "../../Minecraft.Client/ConsoleInputSource.h"

void ServerCommand::notifyAdmins(ConsoleInputSource *src, MinecraftServer *server, const wstring& message)
{
	src->info(message);
	if (server && server->getPlayers())
		server->getPlayers()->broadcastToAllOps(L"\u00A77\u00A7o[" + src->getConsoleName() + L": " + message + L"]");
}

static ConsoleCommandDispatcher *s_dispatcher = NULL;

ConsoleCommandDispatcher *CreateConsoleCommandDispatcher()
{
	ConsoleCommandDispatcher *dispatcher = new ConsoleCommandDispatcher();

	dispatcher->addCommand(new StopCommand());
	dispatcher->addCommand(new TpCommand());

	dispatcher->addCommand(new TimeServerCommand());
	dispatcher->addCommand(new ToggleDownfallServerCommand());
	dispatcher->addCommand(new GiveServerCommand());
	dispatcher->addCommand(new EnchantServerCommand());
	dispatcher->addCommand(new KillServerCommand());
	dispatcher->addCommand(new GameModeServerCommand());
	dispatcher->addCommand(new ListServerCommand());
	dispatcher->addCommand(new KickServerCommand());
	dispatcher->addCommand(new SayCommand());
	dispatcher->addCommand(new MeCommand());
	dispatcher->addCommand(new SeedCommand());
	dispatcher->addCommand(new XpCommand());
	dispatcher->addCommand(new DefaultGameModeCommand());
	dispatcher->addCommand(new SaveAllCommand());
	dispatcher->addCommand(new SaveOffCommand());
	dispatcher->addCommand(new SaveOnCommand());
	dispatcher->addCommand(new DebugCommand());
	dispatcher->addCommand(new OpCommand());
	dispatcher->addCommand(new DeOpCommand());
	dispatcher->addCommand(new BanCommand());
	dispatcher->addCommand(new PardonCommand());
	dispatcher->addCommand(new BanIpCommand());
	dispatcher->addCommand(new PardonIpCommand());
	dispatcher->addCommand(new BanListCommand());
	dispatcher->addCommand(new WhitelistCommand());

	dispatcher->addCommand(new HelpCommand(dispatcher));

	return dispatcher;
}

bool HandleServerCommand(const wstring& rawCmd, ConsoleInputSource *src, MinecraftServer *server)
{
	wstring cmd = rawCmd;

	size_t start = cmd.find_first_not_of(L" \t");
	if (start == wstring::npos) return false;
	cmd = cmd.substr(start);
	size_t end = cmd.find_last_not_of(L" \t");
	if (end != wstring::npos) cmd = cmd.substr(0, end + 1);

	if (!cmd.empty() && cmd[0] == L'/') cmd = cmd.substr(1);
	if (cmd.empty()) return false;

	wstring cmdName;
	wstring cmdArgs;
	size_t spacePos = cmd.find(L' ');
	if (spacePos != wstring::npos)
	{
		cmdName = cmd.substr(0, spacePos);
		cmdArgs = cmd.substr(spacePos + 1);
	}
	else
	{
		cmdName = cmd;
	}

	for (size_t i = 0; i < cmdName.size(); i++)
		cmdName[i] = towlower(cmdName[i]);

	if (!s_dispatcher)
		s_dispatcher = CreateConsoleCommandDispatcher();

	if (cmdName == L"?")
		cmdName = L"help";

	if (!s_dispatcher->performCommand(cmdName, cmdArgs, src, server))
	{
		src->warn(L"Unknown command: " + cmdName + L". Type \"help\" for help.");
	}

	return true;
}

vector<wstring> GetServerCommandCompletions(const wstring& input, MinecraftServer *server)
{
	vector<wstring> results;

	if (!s_dispatcher)
		s_dispatcher = CreateConsoleCommandDispatcher();

	wstring trimmed = input;
	if (!trimmed.empty() && trimmed[0] == L'/')
		trimmed = trimmed.substr(1);

	size_t spacePos = trimmed.find(L' ');

	if (spacePos == wstring::npos)
	{
		wstring partial = trimmed;
		for (size_t i = 0; i < partial.size(); i++)
			partial[i] = towlower(partial[i]);

		const map<wstring, ServerCommand *>& cmds = s_dispatcher->getCommands();
		for (map<wstring, ServerCommand *>::const_iterator it = cmds.begin(); it != cmds.end(); ++it)
		{
			if (it->first.find(partial) == 0)
				results.push_back(it->first);
		}
	}
	else
	{
		wstring lastWord;
		size_t lastSpacePos = trimmed.find_last_of(L' ');
		if (lastSpacePos != wstring::npos && lastSpacePos + 1 < trimmed.size())
			lastWord = trimmed.substr(lastSpacePos + 1);

		wstring partialLower = lastWord;
		for (size_t i = 0; i < partialLower.size(); i++)
			partialLower[i] = towlower(partialLower[i]);

		if (server)
		{
			PlayerList *playerList = server->getPlayers();
			if (playerList)
			{
				for (size_t i = 0; i < playerList->players.size(); i++)
				{
					wstring pname = playerList->players[i]->name;
					wstring pnameLower = pname;
					for (size_t j = 0; j < pnameLower.size(); j++)
						pnameLower[j] = towlower(pnameLower[j]);

					if (pnameLower.find(partialLower) == 0)
						results.push_back(pname);
				}
			}
		}
	}

	return results;
}