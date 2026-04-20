
#include "stdafx.h"
#include "DedicatedServer.h"
#include "ServerLogger.h"
#include "../../Minecraft.World/C4JThread.h"
#include "../../Minecraft.Client/Common/BuildVer.h"
#include "../../Minecraft.World/SharedConstants.h"

#ifdef __linux__
#include <signal.h>
#include <cstdio>
#include <ctime>
#include <locale.h>
#else
#include <fcntl.h>
#include <io.h>
#include <time.h>
#endif

static DedicatedServer g_dedicatedServer;
static volatile bool g_shutdownRequested = false;

#ifdef __linux__

static void SignalHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        printf("\n");
        ServerLog(L"Received shutdown signal, saving and stopping...\n");
        g_shutdownRequested = true;
        MinecraftServer::HaltServer();
    }
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    ServerLog_Init();

    ServerLog(L"Starting Minecraft LCE server version %s (protocol %d)\n", VER_FILEVERSION_STR_W, SharedConstants::NETWORK_PROTOCOL_VERSION);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    if (!g_dedicatedServer.init())
    {
        ServerLog(L"Failed to initialize. Exiting.\n");
        return 1;
    }

    C4JThread *inputThread = new C4JThread(DedicatedServer::consoleInputThread, &g_dedicatedServer, "ConsoleInput");
    inputThread->Run();

    int result = g_dedicatedServer.run();

    g_dedicatedServer.shutdown();
    delete inputThread;

    ServerLog(L"Goodbye.\n");
    ServerLog_Close();
    return result;
}

#else // Windows

static HANDLE g_shutdownCompleteEvent = NULL;

BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        wprintf(L"\n");
        ServerLog(L"Received shutdown signal, saving and stopping...\n");
        g_shutdownRequested = true;
        MinecraftServer::HaltServer();
        if (g_shutdownCompleteEvent)
            WaitForSingleObject(g_shutdownCompleteEvent, 30000);
        return TRUE;
    case CTRL_CLOSE_EVENT:
        FreeConsole();
        g_shutdownRequested = true;
        MinecraftServer::HaltServer();
        if (g_shutdownCompleteEvent)
            WaitForSingleObject(g_shutdownCompleteEvent, 30000);
        ExitProcess(0);
        return TRUE;
    default:
        return FALSE;
    }
}

int wmain(int argc, wchar_t *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleW(L"LCEMP Server");

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    if (hInput != INVALID_HANDLE_VALUE)
    {
        DWORD mode = 0;
        if (GetConsoleMode(hInput, &mode))
        {
            mode |= ENABLE_EXTENDED_FLAGS;
            mode &= ~ENABLE_QUICK_EDIT_MODE;
            mode &= ~ENABLE_MOUSE_INPUT;
            SetConsoleMode(hInput, mode);
        }
    }

    ServerLog_Init();

    ServerLog(L"Starting Minecraft LCE server version %s (protocol %d)\n", VER_FILEVERSION_STR_W, SharedConstants::NETWORK_PROTOCOL_VERSION);

    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    g_shutdownCompleteEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    if (!g_dedicatedServer.init())
    {
        ServerLog(L"Failed to initialize. Exiting.\n");
        return 1;
    }

    C4JThread *inputThread = new C4JThread(DedicatedServer::consoleInputThread, &g_dedicatedServer, "ConsoleInput");
    inputThread->Run();

    int result = g_dedicatedServer.run();

    g_dedicatedServer.shutdown();
    if (g_shutdownCompleteEvent)
    {
        SetEvent(g_shutdownCompleteEvent);
        CloseHandle(g_shutdownCompleteEvent);
    }
    delete inputThread;

    ServerLog(L"Goodbye.\n");
    ServerLog_Close();
    return result;
}

#endif // __linux__