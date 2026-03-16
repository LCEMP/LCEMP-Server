#include "stdafx.h"
#include "TimeServerCommand.h"
#include "ServerCommand.h"
#include "ConsoleCommandSender.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ServerLevel.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.World/GameCommandPacket.h"
#include "../../Minecraft.World/TimeCommand.h"
#include "../../Minecraft.World/ByteArrayOutputStream.h"
#include "../../Minecraft.World/DataOutputStream.h"

#include <sstream>

void TimeServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring subCmd;
	if (!(ss >> subCmd))
	{
		src->warn(L"\u00A7cUsage: /time <set|add> <value>");
		return;
	}

	for (size_t i = 0; i < subCmd.size(); i++)
		subCmd[i] = towlower(subCmd[i]);

	if (subCmd == L"set")
	{
		wstring valueStr;
		if (!(ss >> valueStr))
		{
			src->warn(L"\u00A7cUsage: /time <set|add> <value>");
			return;
		}

		int value;
		if (valueStr == L"day")
			value = 0;
		else if (valueStr == L"night")
			value = 12500;
		else
		{
			value = _wtoi(valueStr.c_str());
			if (value < 0)
			{
				wchar_t errBuf[256];
				swprintf_s(errBuf, 256, L"\u00A7cThe number you have entered (%d) is too small, it must be at least 0", value);
				src->warn(wstring(errBuf));
				return;
			}
		}

		for (int i = 0; i < (int)server->levels.length; i++)
			server->levels[i]->setTimeAndAdjustTileTicks(value);

		wchar_t buf[128];
		swprintf_s(buf, 128, L"Set the time to %d", value);
		ServerCommand::notifyAdmins(src, server, wstring(buf));
	}
	else if (subCmd == L"add")
	{
		wstring valueStr;
		if (!(ss >> valueStr))
		{
			src->warn(L"\u00A7cUsage: /time <set|add> <value>");
			return;
		}
		int value = _wtoi(valueStr.c_str());
		if (value < 0)
		{
			wchar_t errBuf[256];
			swprintf_s(errBuf, 256, L"\u00A7cThe number you have entered (%d) is too small, it must be at least 0", value);
			src->warn(wstring(errBuf));
			return;
		}

		for (int i = 0; i < (int)server->levels.length; i++)
		{
			ServerLevel *level = server->levels[i];
			level->setTimeAndAdjustTileTicks(level->getTime() + value);
		}

		wchar_t buf[128];
		swprintf_s(buf, 128, L"Added %d to the time", value);
		ServerCommand::notifyAdmins(src, server, wstring(buf));
	}
	else
	{
		src->warn(L"\u00A7cUsage: /time <set|add> <value>");
	}
}
