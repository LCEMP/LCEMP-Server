#include "stdafx.h"
#include "ConsoleCommandSender.h"
#include "../../Minecraft.Client/ConsoleInputSource.h"

void ConsoleCommandSender::sendMessage(const wstring& message, ChatPacket::EChatPacketMessage type, int customData, const wstring& additionalMessage)
{
	src->info(message);
}

bool ConsoleCommandSender::hasPermission(EGameCommand command)
{
	return true;
}
