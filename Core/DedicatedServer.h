#pragma once

#include "../../Minecraft.Client/ConsoleInputSource.h"
#include "../../Minecraft.Client/MinecraftServer.h"
#include "ServerProperties.h"

class DedicatedServer : public ConsoleInputSource
{
public:
    DedicatedServer();
    ~DedicatedServer();

    bool init();

    int run();

    void shutdown();

    virtual void info(const wstring& string);
    virtual void warn(const wstring& string);
    virtual wstring getConsoleName();

    static int consoleInputThread(void *param);

private:
    void processConsoleInput();

    bool m_running;
    ServerProperties m_properties;
};