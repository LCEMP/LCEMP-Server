#include "stdafx.h"
#include "DebugCommand.h"
#include "ServerCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"

#include <sstream>
#include <time.h>

static bool s_profiling = false;
static time_t s_profilingStart = 0;
static int s_profilingStartTick = 0;

void DebugCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring subCmd;
	if (!(ss >> subCmd))
	{
		src->warn(L"Usage: /debug <start|stop>");
		return;
	}

	if (subCmd == L"start")
	{
		if (s_profiling)
		{
			src->warn(L"Debug profiling is already started");
			return;
		}
		s_profiling = true;
		s_profilingStart = time(NULL);
		s_profilingStartTick = server->tickCount;
		ServerCommand::notifyAdmins(src, server, L"Starting debug profiling");
	}
	else if (subCmd == L"stop")
	{
		if (!s_profiling)
		{
			src->warn(L"Debug profiling is not started");
			return;
		}
		s_profiling = false;
		time_t elapsed = time(NULL) - s_profilingStart;
		int ticks = server->tickCount - s_profilingStartTick;
		float tps = elapsed > 0 ? (float)ticks / (float)elapsed : 0.0f;
		wchar_t buf[256];
		swprintf_s(buf, 256, L"Stopped debug profiling after %.2f seconds and %d ticks (%.2f ticks per second)", (float)elapsed, ticks, tps);
		ServerCommand::notifyAdmins(src, server, wstring(buf));
	}
	else
	{
		src->warn(L"Usage: /debug <start|stop>");
	}
}
