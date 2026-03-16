#include "stdafx.h"
#include "SeedCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/ServerLevel.h"
#include "../../Minecraft.World/Level.h"

void SeedCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	__int64 seed = server->getLevel(0)->getSeed();
	wchar_t buf[64];
	swprintf_s(buf, 64, L"Seed: %lld", seed);
	src->info(wstring(buf));
}
