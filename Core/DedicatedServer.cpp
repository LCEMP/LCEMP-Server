#include "stdafx.h"
#include "DedicatedServer.h"

#include "../../Minecraft.Client/MinecraftServer.h"
#include "../../Minecraft.Client/Minecraft.h"
#include "../../Minecraft.Client/ProgressRenderer.h"
#include "../../Minecraft.Client/GameRenderer.h"
#include "../../Minecraft.Client/Options.h"
#include "../../Minecraft.Client/User.h"
#include "../../Minecraft.Client/StatsCounter.h"
#include "../../Minecraft.Client/Windows64/Network/WinsockNetLayer.h"
#include "../../Minecraft.Client/Common/Network/PlatformNetworkManagerStub.h"
#include "../../Minecraft.World/Minecraft.World.h"
#include "../../Minecraft.World/System.h"
#include "../../Minecraft.World/AABB.h"
#include "../../Minecraft.World/Vec3.h"
#include "../../Minecraft.World/IntCache.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/OldChunkStorage.h"
#include "../../Minecraft.World/Level.h"
#include "../../Minecraft.World/Tile.h"
#include "../../Minecraft.World/Entity.h"
#include "../../Minecraft.World/SharedConstants.h"
#include "../../Minecraft.World/ChunkSource.h"
#include "ServerLogger.h"
#include "ServerLists.h"
#include "../Commands/ServerCommands.h"

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#else
#include <conio.h>
#endif

DedicatedServer::DedicatedServer()
    : m_running(false)
{
}

DedicatedServer::~DedicatedServer()
{
}

bool DedicatedServer::init()
{
    if (!m_properties.load(L"server.properties"))
    {
        ServerLog(L"server.properties not found, generating defaults\n");
        m_properties.save(L"server.properties");
    }

    ServerLog(L"Loading properties\n");
    ServerLog(L"Default game type: %s\n", m_properties.gamemode == 0 ? L"SURVIVAL" : L"CREATIVE");
    ServerLog(L"Max players: %d\n", m_properties.maxPlayers);
    ServerLog(L"Difficulty: %d\n", m_properties.difficulty);
    ServerLog(L"Level size: %s\n", m_properties.levelSize.c_str());

    ServerLists_Init(m_properties.whiteList, &m_properties);

    extern bool g_voiceChatEnabled;
    g_voiceChatEnabled = m_properties.voiceChat;

    extern bool g_ServerAdvertiseLAN;
    g_ServerAdvertiseLAN = m_properties.advertiseLan;

    extern int g_ServerMaxPlayers;
    g_ServerMaxPlayers = m_properties.maxPlayers;

    extern char g_ServerBindAddress[256];
    if (!m_properties.serverIp.empty())
    {
        wcstombs(g_ServerBindAddress, m_properties.serverIp.c_str(), sizeof(g_ServerBindAddress) - 1);
        g_ServerBindAddress[sizeof(g_ServerBindAddress) - 1] = '\0';
    }
    else
    {
        g_ServerBindAddress[0] = '\0';
    }
    if (g_voiceChatEnabled && m_properties.maxPlayers > 8)
    {
        ServerLog(L"Voice chat has been disabled: max-players (%d) exceeds the maximum supported for voice chat (8)\n", m_properties.maxPlayers);
        g_voiceChatEnabled = false;
    }
    else if (g_voiceChatEnabled)
    {
        ServerLog(L"Voice chat is enabled\n");
    }

    AABB::CreateNewThreadStorage();
    Vec3::CreateNewThreadStorage();
    IntCache::CreateNewThreadStorage();
    Compression::CreateNewThreadStorage();
    OldChunkStorage::CreateNewThreadStorage();
    Level::enableLightingCache();
    Tile::CreateNewThreadStorage();

    MinecraftWorld_RunStaticCtors();

    {
        wchar_t exePath[MAX_PATH];
        if (GetModuleFileNameW(NULL, exePath, MAX_PATH))
        {
            wstring exeDir(exePath);
            size_t lastSlash = exeDir.find_last_of(L"\\/");
            if (lastSlash != wstring::npos)
                exeDir = exeDir.substr(0, lastSlash);
            SetCurrentDirectoryW(exeDir.c_str());
#ifdef __linux__
            wstring gameHDDPath = exeDir + L"/Linux/GameHDD";
            CreateDirectoryW((exeDir + L"/Linux").c_str(), NULL);
#else
            wstring gameHDDPath = exeDir + L"\\Windows64\\GameHDD";
            CreateDirectoryW((exeDir + L"\\Windows64").c_str(), NULL);
#endif
            CreateDirectoryW(gameHDDPath.c_str(), NULL);
        }
    }

    StorageManager.Init(1, app.GetString(IDS_DEFAULT_SAVENAME), "savegame.dat", FIFTY_ONE_MB, NULL, NULL, "MinecraftDedicatedServer");
    StorageManager.SetMaxSaves(99);
    StorageManager.StoreTMSPathName();

    g_NetworkManager.Initialise();

    for (int i = 0; i < MINECRAFT_NET_MAX_PLAYERS; i++)
    {
        IQNet::m_player[i].m_smallId = (BYTE)i;
        IQNet::m_player[i].m_isRemote = false;
        IQNet::m_player[i].m_isHostPlayer = (i == 0);
        swprintf_s(IQNet::m_player[i].m_gamertag, 32, L"Server%d", i);
    }
    wcscpy_s(IQNet::m_player[0].m_gamertag, 32, m_properties.motd.c_str());

    WinsockNetLayer::Initialize();

    app.SetGameHostOption(eGameHostOption_Difficulty, m_properties.difficulty);
    app.SetGameHostOption(eGameHostOption_GameType, m_properties.gamemode);
    app.SetGameHostOption(eGameHostOption_PvP, m_properties.pvp ? 1 : 0);
    app.SetGameHostOption(eGameHostOption_TrustPlayers, m_properties.trustPlayers ? 1 : 0);
    app.SetGameHostOption(eGameHostOption_FireSpreads, m_properties.fireSpreads ? 1 : 0);
    app.SetGameHostOption(eGameHostOption_TNT, m_properties.tntExplodes ? 1 : 0);
    app.SetGameHostOption(eGameHostOption_Structures, m_properties.structures ? 1 : 0);
    app.SetGameHostOption(eGameHostOption_Gamertags, m_properties.showGamertags ? 1 : 0);

    Minecraft *pMinecraft = new Minecraft(NULL, NULL, NULL, 1, 1, false);
    pMinecraft->options = new Options(pMinecraft, File(L"."));
    pMinecraft->progressRenderer = new ProgressRenderer(pMinecraft);
    pMinecraft->gameRenderer = new GameRenderer(pMinecraft);
    for (int i = 0; i < 4; i++)
        pMinecraft->stats[i] = new StatsCounter();

    {
        static DWORD dwProfileSettingsA[5] = {0, 0, 0, 0, 0};
        ProfileManager.Initialise(TITLEID_MINECRAFT,
            app.m_dwOfferID,
            PROFILE_VERSION_10,
            5,
            4,
            dwProfileSettingsA,
            app.GAME_DEFINED_PROFILE_DATA_BYTES * XUSER_MAX_COUNT,
            &app.uiGameDefinedDataChangedBitmask);
    }

    app.InitGameSettings();

    if (!m_properties.serverIp.empty())
        ServerLog(L"Starting Minecraft server on %s:%d\n", m_properties.serverIp.c_str(), m_properties.serverPort);
    else
        ServerLog(L"Starting Minecraft server on *:%d\n", m_properties.serverPort);
    m_running = true;
    return true;
}

int DedicatedServer::run()
{
    if (!m_running)
    {
        ServerWarn(L"Server not initialized!\n");
        return 1;
    }

    ServerLog(L"Preparing level \"%s\"\n", m_properties.levelName.c_str());

    StorageManager.GetSavesInfo(0, NULL, NULL, NULL);
    PSAVE_DETAILS pSaveDetails = StorageManager.ReturnSavesInfo();
    if (pSaveDetails && pSaveDetails->iSaveC > 0)
    {
        int bestIdx = 0;
        time_t bestTime = pSaveDetails->SaveInfoA[0].metaData.modifiedTime;
        for (int i = 1; i < pSaveDetails->iSaveC; i++)
        {
            if (pSaveDetails->SaveInfoA[i].metaData.modifiedTime > bestTime)
            {
                bestTime = pSaveDetails->SaveInfoA[i].metaData.modifiedTime;
                bestIdx = i;
            }
        }
        ServerLog(L"Loading existing world from save \"%S\"\n", pSaveDetails->SaveInfoA[bestIdx].UTF8SaveFilename);
        StorageManager.LoadSaveData(&pSaveDetails->SaveInfoA[bestIdx], NULL, NULL);
    }
    else
    {
        ServerLog(L"No existing save found, creating new world\n");
        wstring title = m_properties.levelName;
        StorageManager.SetSaveTitle(title.c_str());
    }

    g_NetworkManager.HostGame(1, true, false, (unsigned char)m_properties.maxPlayers, 0);

    extern CPlatformNetworkManagerStub *g_pPlatformNetworkManager;
    g_pPlatformNetworkManager->NotifyPlayerJoined(&IQNet::m_player[0]);

    g_NetworkManager.ServerReadyCreate(true);
    g_NetworkManager.ServerStoppedCreate(true);

    NetworkGameInitData initData;
    initData.seed = m_properties.levelSeed;
    initData.saveData = NULL;
    initData.settings = 0;
    initData.levelGen = NULL;
    initData.texturePackId = 0;
    initData.findSeed = (m_properties.levelSeed == 0);

    if (m_properties.levelSize == L"classic")
    {
        initData.xzSize = 1 * 54;
        initData.hellScale = 3;
    }
    else if (m_properties.levelSize == L"small")
    {
        initData.xzSize = 1 * 64;
        initData.hellScale = 3;
    }
    else if (m_properties.levelSize == L"medium")
    {
        initData.xzSize = 3 * 64;
        initData.hellScale = 6;
    }
    else
    {
        initData.xzSize = LEVEL_MAX_WIDTH;
        initData.hellScale = HELL_LEVEL_MAX_SCALE;
    }

    AABB::CreateNewThreadStorage();
    Vec3::CreateNewThreadStorage();
    IntCache::CreateNewThreadStorage();
    Compression::UseDefaultThreadStorage();
    Entity::useSmallIds();
    Level::enableLightingCache();
    Tile::CreateNewThreadStorage();

    ServerLog(L"Preparing start region for level 0\n");
    MinecraftServer::main(m_properties.levelSeed, &initData);

    g_NetworkManager.ServerReadyDestroy();
    g_NetworkManager.ServerStoppedDestroy();

    ServerLog(L"Server stopped\n");
    return 0;
}

void DedicatedServer::shutdown()
{
    ServerLog(L"Stopping server\n");
    m_running = false;
    MinecraftServer::HaltServer();

    Tile::ReleaseThreadStorage();
    IntCache::ReleaseThreadStorage();
    AABB::ReleaseThreadStorage();
    Vec3::ReleaseThreadStorage();
    Level::destroyLightingCache();

    g_NetworkManager.Terminate();
    ServerLog(L"Shutdown complete\n");
}

void DedicatedServer::info(const wstring& string)
{
    ServerLog(L"%s\n", string.c_str());
}

void DedicatedServer::warn(const wstring& string)
{
    ServerWarn(L"%s\n", string.c_str());
}

wstring DedicatedServer::getConsoleName()
{
    return L"Server";
}

int DedicatedServer::consoleInputThread(void *param)
{
    DedicatedServer *server = (DedicatedServer *)param;

#ifdef __linux__
    struct termios origTermios;
    tcgetattr(STDIN_FILENO, &origTermios);
    {
        struct termios raw = origTermios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
#endif

    wstring buffer;
    vector<wstring> history;
    int historyPos = -1;
    wstring savedBuffer;
    ServerLog_RegisterInput(&buffer);

    auto clearLine = [&]() {
        if (buffer.empty()) return;
#ifdef __linux__
        for (size_t i = 0; i < buffer.size(); i++)
            printf("\b \b");
        fflush(stdout);
#else
        for (size_t i = 0; i < buffer.size(); i++)
            wprintf(L"\b \b");
#endif
    };

    auto redrawLine = [&]() {
#ifdef __linux__
        printf("%ls", buffer.c_str());
        fflush(stdout);
#else
        wprintf(L"%s", buffer.c_str());
#endif
    };

    while (server->m_running)
    {
#ifdef __linux__
        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) break;
        wchar_t wc = (wchar_t)(unsigned char)c;
#else
        wchar_t wc = _getwch();
#endif

        if (wc == L'\t')
        {
            ServerLog_LockOutput();
            MinecraftServer *mcServer = MinecraftServer::getInstance();
            vector<wstring> completions = GetServerCommandCompletions(buffer, mcServer);

            if (completions.size() == 1)
            {
                size_t lastSpace = buffer.find_last_of(L' ');
                wstring currentWord = (lastSpace == wstring::npos) ? buffer : buffer.substr(lastSpace + 1);
                wstring toAppend = completions[0].substr(currentWord.size()) + L" ";
                buffer += toAppend;
#ifdef __linux__
                printf("%ls", toAppend.c_str());
                fflush(stdout);
#else
                wprintf(L"%s", toAppend.c_str());
#endif
            }
            else if (completions.size() > 1)
            {
                wstring common = completions[0];
                for (size_t i = 1; i < completions.size(); i++)
                {
                    size_t j = 0;
                    while (j < common.size() && j < completions[i].size() &&
                           towlower(common[j]) == towlower(completions[i][j]))
                        j++;
                    common = common.substr(0, j);
                }

                size_t lastSpace = buffer.find_last_of(L' ');
                wstring currentWord = (lastSpace == wstring::npos) ? buffer : buffer.substr(lastSpace + 1);

                if (common.size() > currentWord.size())
                {
                    wstring toAppend = common.substr(currentWord.size());
                    buffer += toAppend;
#ifdef __linux__
                    printf("%ls", toAppend.c_str());
                    fflush(stdout);
#else
                    wprintf(L"%s", toAppend.c_str());
#endif
                }
                else
                {
#ifdef __linux__
                    printf("\n");
                    for (size_t i = 0; i < completions.size(); i++)
                        printf("%ls ", completions[i].c_str());
                    printf("\n%ls", buffer.c_str());
                    fflush(stdout);
#else
                    wprintf(L"\n");
                    for (size_t i = 0; i < completions.size(); i++)
                        wprintf(L"%s ", completions[i].c_str());
                    wprintf(L"\n%s", buffer.c_str());
#endif
                }
            }
            ServerLog_UnlockOutput();
        }
        else if (wc == L'\r' || wc == L'\n')
        {
            ServerLog_LockOutput();
#ifdef __linux__
            printf("\n");
            fflush(stdout);
#else
            wprintf(L"\n");
#endif
            wstring cmd = buffer;
            buffer.clear();
            ServerLog_UnlockOutput();
            if (!cmd.empty())
            {
                if (history.empty() || history.back() != cmd)
                    history.push_back(cmd);
                historyPos = -1;

                if (cmd == L"stop" || cmd == L"quit" || cmd == L"exit")
                {
                    ServerLog(L"Stopping the server\n");
                    MinecraftServer::HaltServer();
                    server->m_running = false;
                    break;
                }
                MinecraftServer *mcServer = MinecraftServer::getInstance();
                if (mcServer)
                    mcServer->handleConsoleInput(cmd, server);
            }
        }
        else if (wc == 127 || wc == 8)
        {
            if (!buffer.empty())
            {
                ServerLog_LockOutput();
                buffer.pop_back();
#ifdef __linux__
                printf("\b \b");
                fflush(stdout);
#else
                wprintf(L"\b \b");
#endif
                ServerLog_UnlockOutput();
            }
        }
#ifdef __linux__
        else if (c == 27)
        {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (seq[0] != '[') continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            if (seq[1] == 'A')
            {
                if (history.empty()) continue;
                if (historyPos == -1)
                {
                    savedBuffer = buffer;
                    historyPos = (int)history.size() - 1;
                }
                else if (historyPos > 0)
                {
                    historyPos--;
                }
                ServerLog_LockOutput();
                clearLine();
                buffer = history[historyPos];
                redrawLine();
                ServerLog_UnlockOutput();
            }
            else if (seq[1] == 'B')
            {
                if (historyPos == -1) continue;
                ServerLog_LockOutput();
                clearLine();
                if (historyPos < (int)history.size() - 1)
                {
                    historyPos++;
                    buffer = history[historyPos];
                }
                else
                {
                    historyPos = -1;
                    buffer = savedBuffer;
                }
                redrawLine();
                ServerLog_UnlockOutput();
            }
        }
#else
        else if (wc == 0 || wc == 0xE0)
        {
            wchar_t ext = _getwch();
            if (ext == 72)
            {
                if (history.empty()) continue;
                if (historyPos == -1)
                {
                    savedBuffer = buffer;
                    historyPos = (int)history.size() - 1;
                }
                else if (historyPos > 0)
                {
                    historyPos--;
                }
                ServerLog_LockOutput();
                clearLine();
                buffer = history[historyPos];
                redrawLine();
                ServerLog_UnlockOutput();
            }
            else if (ext == 80)
            {
                if (historyPos == -1) continue;
                ServerLog_LockOutput();
                clearLine();
                if (historyPos < (int)history.size() - 1)
                {
                    historyPos++;
                    buffer = history[historyPos];
                }
                else
                {
                    historyPos = -1;
                    buffer = savedBuffer;
                }
                redrawLine();
                ServerLog_UnlockOutput();
            }
        }
#endif
        else if (wc >= 32)
        {
            ServerLog_LockOutput();
            buffer += wc;
#ifdef __linux__
            printf("%lc", wc);
            fflush(stdout);
#else
            wprintf(L"%c", wc);
#endif
            ServerLog_UnlockOutput();
        }
    }

    ServerLog_UnregisterInput();
#ifdef __linux__
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
#endif
    return 0;
}