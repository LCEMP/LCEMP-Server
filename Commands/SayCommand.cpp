#include "stdafx.h"
#include "SayCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.World/ChatPacket.h"

void SayCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	if (args.empty())
	{
		src->warn(L"Usage: /say <message ...>");
		return;
	}
	wstring msg = L"[Server] " + args;
	server->getPlayers()->broadcastAll(shared_ptr<Packet>(new ChatPacket(msg)));
	src->info(L"\u00A7d[Server] " + args);
}
