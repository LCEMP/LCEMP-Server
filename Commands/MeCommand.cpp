#include "stdafx.h"
#include "MeCommand.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerConnection.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.World/ChatPacket.h"

void MeCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	if (args.empty())
	{
		src->warn(L"Usage: /me <action ...>");
		return;
	}
	wstring senderName = L"Server";
	if (src != NULL)
	{
		senderName = src->getConsoleName();
		if (senderName.empty())
		{
			senderName = L"Server";
		}
	}

	wstring msg = L"* " + senderName + L" " + args;
	server->getPlayers()->broadcastAll(shared_ptr<Packet>(new ChatPacket(msg)));

	// player sources already receive the broadcast chat line above
	if (dynamic_cast<PlayerConnection *>(src) == NULL)
	{
		src->info(msg);
	}
}
