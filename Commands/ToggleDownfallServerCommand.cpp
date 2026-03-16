#include "stdafx.h"
#include "ToggleDownfallServerCommand.h"
#include "ServerCommand.h"
#include "ConsoleCommandSender.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.World/CommandDispatcher.h"
#include "../../Minecraft.World/ByteArrayOutputStream.h"

void ToggleDownfallServerCommand::execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server)
{
	shared_ptr<ConsoleCommandSender> sender(new ConsoleCommandSender(src));
	CommandDispatcher *dispatcher = server->getCommandDispatcher();
	dispatcher->performCommand(sender, eGameCommand_ToggleDownfall, byteArray());
	ServerCommand::notifyAdmins(src, server, L"Toggled downfall");
}
