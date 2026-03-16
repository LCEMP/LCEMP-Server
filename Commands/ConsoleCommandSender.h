#pragma once

#include "../../Minecraft.World/CommandSender.h"

class ConsoleInputSource;

class ConsoleCommandSender : public CommandSender
{
private:
	ConsoleInputSource *src;

public:
	ConsoleCommandSender(ConsoleInputSource *src) : src(src) {}

	void sendMessage(const wstring& message, ChatPacket::EChatPacketMessage type = ChatPacket::e_ChatCustom, int customData = -1, const wstring& additionalMessage = L"");
	bool hasPermission(EGameCommand command);
};
