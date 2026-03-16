#include "stdafx.h"
#include "GiveServerCommand.h"
#include "ConsoleCommandSender.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/PlayerList.h"
#include "../../Minecraft.Client/ServerPlayer.h"
#include "../../Minecraft.World/CommandDispatcher.h"
#include "../../Minecraft.World/ByteArrayOutputStream.h"
#include "../../Minecraft.World/DataOutputStream.h"

#include <sstream>

void GiveServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	wstringstream ss(args);
	wstring playerName;
	int item = 0;
	int amount = 1;
	int aux = 0;

	if (!(ss >> playerName >> item))
	{
		src->warn(L"\u00A7cUsage: /give <player> <item id> [amount] [data]");
		return;
	}

	if (ss >> amount)
	{
		if (amount < 1 || amount > 64)
		{
			src->warn(L"\u00A7cItem amount must be between 1 and 64");
			return;
		}
	}
	ss >> aux;

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
	dos.writeInt(item);
	dos.writeInt(amount);
	dos.writeInt(aux);
	dos.writeUTF(L"");

	shared_ptr<ConsoleCommandSender> sender(new ConsoleCommandSender(src));
	CommandDispatcher *dispatcher = server->getCommandDispatcher();
	dispatcher->performCommand(sender, eGameCommand_Give, baos.toByteArray());
}
