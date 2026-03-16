#pragma once


#ifdef __linux__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <pthread.h>
#include <cstring>

#include "../../Minecraft.Client/Common/Network/NetworkPlayerInterface.h"

typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

inline int closesocket(SOCKET s) { return close(s); }

#define WIN64_NET_DEFAULT_PORT       25565
#define WIN64_NET_MAX_CLIENTS        7
#define WIN64_NET_RECV_BUFFER_SIZE   65536
#define WIN64_NET_MAX_PACKET_SIZE    (3 * 1024 * 1024)
#define WIN64_LAN_DISCOVERY_PORT     25566
#define WIN64_LAN_BROADCAST_MAGIC    0x4D434C4E
#define WIN64_LAN_BROADCAST_PLAYERS  8

class Socket;

#pragma pack(push, 1)
struct Win64LANBroadcast
{
    DWORD magic;
    uint16_t netVersion;
    uint16_t gamePort;
    uint16_t hostName[32];
    uint8_t playerCount;
    uint8_t maxPlayers;
    DWORD gameHostSettings;
    DWORD texturePackParentId;
    uint8_t subTexturePackId;
    uint8_t isJoinable;
    uint8_t isDedicatedServer;
    char playerNames[WIN64_LAN_BROADCAST_PLAYERS][32]; // XUSER_NAME_SIZE
};
#pragma pack(pop)

struct Win64LANSession
{
    char hostIP[64];
    int hostPort;
    wchar_t hostName[32];
    unsigned short netVersion;
    unsigned char playerCount;
    unsigned char maxPlayers;
    unsigned int gameHostSettings;
    unsigned int texturePackParentId;
    unsigned char subTexturePackId;
    bool isJoinable;
    bool isDedicatedServer;
    DWORD lastSeenTick;
    char playerNames[WIN64_LAN_BROADCAST_PLAYERS][32];
};

struct Win64RemoteConnection
{
    SOCKET tcpSocket;
    uint8_t smallId;
    pthread_t recvThread;
    bool recvThreadActive;
    volatile bool active;
    pthread_mutex_t sendLock;
};

class WinsockNetLayer
{
public:
    static bool Initialize();
    static void Shutdown();

    static bool HostGame(int port);
    static bool JoinGame(const char *ip, int port);

    static bool SendToSmallId(uint8_t targetSmallId, const void *data, int dataSize);
    static bool SendOnSocket(SOCKET sock, const void *data, int dataSize);

    static bool IsHosting() { return s_isHost; }
    static bool IsConnected() { return s_connected; }
    static bool IsActive() { return s_active; }

    static uint8_t GetLocalSmallId() { return s_localSmallId; }
    static uint8_t GetHostSmallId() { return s_hostSmallId; }

    static SOCKET GetSocketForSmallId(uint8_t smallId);
    static std::string GetIPForSmallId(uint8_t smallId);

    static void HandleDataReceived(uint8_t fromSmallId, uint8_t toSmallId, unsigned char *data, unsigned int dataSize);
    static void FlushPendingData();

    static bool PopDisconnectedSmallId(uint8_t *outSmallId);
    static void PushFreeSmallId(uint8_t smallId);
    static void CloseConnectionBySmallId(uint8_t smallId);

    static bool PopPendingJoinSmallId(uint8_t *outSmallId);

    static bool IsSmallIdConnected(uint8_t smallId);

    static bool StartAdvertising(int gamePort, const wchar_t *hostName, unsigned int gameSettings, unsigned int texPackId, unsigned char subTexId, unsigned short netVer);
    static void StopAdvertising();
    static void UpdateAdvertisePlayerCount(uint8_t count);
    static void UpdateAdvertiseJoinable(bool joinable);
    static void UpdateAdvertisePlayerNames(uint8_t count, const char playerNames[][32]);

    static bool StartDiscovery();
    static void StopDiscovery();
    static std::vector<Win64LANSession> GetDiscoveredSessions();

    static int GetHostPort() { return s_hostGamePort; }

private:
    static void* AcceptThreadProc(void* param);
    static void* RecvThreadProc(void* param);
    static void* ClientRecvThreadProc(void* param);
    static void* AdvertiseThreadProc(void* param);
    static void* DiscoveryThreadProc(void* param);

    static SOCKET s_listenSocket;
    static SOCKET s_hostConnectionSocket;
    static pthread_t s_acceptThread;
    static bool s_acceptThreadActive;
    static pthread_t s_clientRecvThread;
    static bool s_clientRecvThreadActive;

    static bool s_isHost;
    static bool s_connected;
    static bool s_active;
    static bool s_initialized;

    static uint8_t s_localSmallId;
    static uint8_t s_hostSmallId;
    static uint8_t s_nextSmallId;

    static pthread_mutex_t s_sendLock;
    static pthread_mutex_t s_connectionsLock;

    static Win64RemoteConnection s_connections[WIN64_NET_MAX_CLIENTS + 1];

    static SOCKET s_advertiseSock;
    static pthread_t s_advertiseThread;
    static bool s_advertiseThreadActive;
    static volatile bool s_advertising;
    static Win64LANBroadcast s_advertiseData;
    static pthread_mutex_t s_advertiseLock;
    static int s_hostGamePort;

    static SOCKET s_discoverySock;
    static pthread_t s_discoveryThread;
    static bool s_discoveryThreadActive;
    static volatile bool s_discovering;
    static pthread_mutex_t s_discoveryLock;
    static std::vector<Win64LANSession> s_discoveredSessions;

    static pthread_mutex_t s_disconnectLock;
    static std::vector<uint8_t> s_disconnectedSmallIds;

    static pthread_mutex_t s_pendingJoinLock;
    static std::vector<uint8_t> s_pendingJoinSmallIds;

    static pthread_mutex_t s_freeSmallIdLock;
    static std::vector<uint8_t> s_freeSmallIds;

    static pthread_mutex_t s_earlyDataLock;
    static std::vector<uint8_t> s_earlyDataBuffers[WIN64_NET_MAX_CLIENTS + 1];
};

extern bool g_Win64MultiplayerHost;
extern bool g_Win64MultiplayerJoin;
extern int g_Win64MultiplayerPort;
extern char g_Win64MultiplayerIP[256];

#endif // __linux__
