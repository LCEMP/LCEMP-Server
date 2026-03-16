#include "stdafx.h"
#include "EnchantServerCommand.h"
#include "ConsoleCommandSender.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.World/CommandDispatcher.h"
#include "../../Minecraft.World/ByteArrayOutputStream.h"
#include "../../Minecraft.World/DataOutputStream.h"

#include <sstream>

void EnchantServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;
	int enchantmentId = 0;
	int enchantmentLevel = 1;

	if (!(ss >> playerName >> enchantmentId))
	{
		src->warn(L"\u00A7cUsage: /enchant <player> <enchantment ID> [level]");
		return;
	}

	ss >> enchantmentLevel;

	PlayerList *players = server->getPlayers();
	shared_ptr<ServerPlayer> player = players->getPlayer(playerName);
	if (!player)
	{
		src->warn(L"\u00A7cThat player cannot be found");
		return;
	}

	ByteArrayOutputStream baos;
	DataOutputStream dos(&baos);

	dos.writePlayerUID(player->getXuid());
	dos.writeInt(enchantmentId);
	dos.writeInt(enchantmentLevel);

	shared_ptr<ConsoleCommandSender> sender(new ConsoleCommandSender(src));
	CommandDispatcher *dispatcher = server->getCommandDispatcher();
	dispatcher->performCommand(sender, eGameCommand_EnchantItem, baos.toByteArray());
}
