#include "stdafx.h"
#include "HelpCommand.h"
#include "ConsoleCommandDispatcher.h"

#include "../../Minecraft.Client/ConsoleInputSource.h"
#include <sstream>
#include <vector>
#include <algorithm>

HelpCommand::HelpCommand(ConsoleCommandDispatcher *dispatcher)
	: dispatcher(dispatcher)
{
}

void HelpCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	const map<wstring, ServerCommand *>& commandMap = dispatcher->getCommands();

	vector<ServerCommand *> list;
	for (AUTO_VAR(it, commandMap.begin()); it != commandMap.end(); ++it)
		list.push_back(it->second);

	sort(list.begin(), list.end(), [](ServerCommand *a, ServerCommand *b) {
		return a->getName() < b->getName();
	});

	int itemsPerPage = 7;
	int totalPages = (int)list.size() / itemsPerPage;
	int page = 0;

	wstringstream ss(args);
	wstring arg;
	if (ss >> arg)
	{
		wstringstream numTest(arg);
		int num;
		if (numTest >> num && numTest.eof())
		{
			if (num < 1) num = 1;
			if (num > totalPages + 1) num = totalPages + 1;
			page = num - 1;
		}
		else
		{
			for (size_t i = 0; i < arg.size(); i++)
				arg[i] = towlower(arg[i]);

			AUTO_VAR(it, commandMap.find(arg));
			if (it != commandMap.end())
			{
				src->info(L"\u00A7cUsage: " + it->second->getUsage());
				return;
			}

			src->warn(L"\u00A7cUnknown command. Try /help for a list of commands");
			return;
		}
	}

	int endIndex = (int)list.size();
	if ((page + 1) * itemsPerPage < endIndex)
		endIndex = (page + 1) * itemsPerPage;

	wchar_t buf[256];
	swprintf_s(buf, 256, L"\u00A72--- Showing help page %d of %d (/help <page>) ---", page + 1, totalPages + 1);
	src->info(wstring(buf));

	for (int k = page * itemsPerPage; k < endIndex; k++)
		src->info(list[k]->getUsage());

	if (page == 0)
		src->info(L"\u00A7aTip: Use the <tab> key while typing a command to auto-complete the command or its arguments");
}
