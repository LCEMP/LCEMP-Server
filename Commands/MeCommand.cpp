#include "stdafx.h"
#include "MeCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.World/ChatPacket.h"

void MeCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	if (args.empty())
	{
		src->warn(L"Usage: /me <action ...>");
		return;
	}
	wstring msg = L"* Server " + args;
	server->getPlayers()->broadcastAll(shared_ptr<Packet>(new ChatPacket(msg)));
	src->info(msg);
}
