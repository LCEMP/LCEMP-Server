#pragma once

class MinecraftServer;
class ConsoleInputSource;

class ServerCommand
{
public:
	virtual ~ServerCommand() {}
	virtual wstring getName() = 0;
	virtual wstring getUsage() = 0;
	virtual void execute(const wstring& args, ConsoleInputSource *src, MinecraftServer *server) = 0;

	static void notifyAdmins(ConsoleInputSource *src, MinecraftServer *server, const wstring& message);
};
