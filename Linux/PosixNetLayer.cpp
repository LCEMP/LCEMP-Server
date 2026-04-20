
#include "stdafx.h"

#ifdef __linux__

#include "PosixNetLayer.h"
#include "../../Minecraft.Client/Common/Network/PlatformNetworkManagerStub.h"
#include "../../Minecraft.World/Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <cerrno>
#include <cstring>
#include <time.h>

SOCKET WinsockNetLayer::s_listenSocket = INVALID_SOCKET;
SOCKET WinsockNetLayer::s_hostConnectionSocket = INVALID_SOCKET;
pthread_t WinsockNetLayer::s_acceptThread;
bool WinsockNetLayer::s_acceptThreadActive = false;
pthread_t WinsockNetLayer::s_clientRecvThread;
bool WinsockNetLayer::s_clientRecvThreadActive = false;

int WinsockNetLayer::s_epollFd = -1;
pthread_t WinsockNetLayer::s_epollThreads[WIN64_NET_IO_THREADS];
bool WinsockNetLayer::s_epollThreadsActive = false;

bool WinsockNetLayer::s_isHost = false;
bool WinsockNetLayer::s_connected = false;
bool WinsockNetLayer::s_active = false;
bool WinsockNetLayer::s_initialized = false;

uint8_t WinsockNetLayer::s_localSmallId = 0;
uint8_t WinsockNetLayer::s_hostSmallId = 0;
std::atomic<uint8_t> WinsockNetLayer::s_nextSmallId(1);

pthread_mutex_t WinsockNetLayer::s_sendLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t WinsockNetLayer::s_connectionsLock = PTHREAD_MUTEX_INITIALIZER;

std::vector<Win64RemoteConnection> WinsockNetLayer::s_connections;

SOCKET WinsockNetLayer::s_advertiseSock = INVALID_SOCKET;
pthread_t WinsockNetLayer::s_advertiseThread;
bool WinsockNetLayer::s_advertiseThreadActive = false;
volatile bool WinsockNetLayer::s_advertising = false;
Win64LANBroadcast WinsockNetLayer::s_advertiseData = {};
pthread_mutex_t WinsockNetLayer::s_advertiseLock = PTHREAD_MUTEX_INITIALIZER;
int WinsockNetLayer::s_hostGamePort = WIN64_NET_DEFAULT_PORT;

SOCKET WinsockNetLayer::s_discoverySock = INVALID_SOCKET;
SOCKET WinsockNetLayer::s_discoveryLegacySock = INVALID_SOCKET;
pthread_t WinsockNetLayer::s_discoveryThread;
bool WinsockNetLayer::s_discoveryThreadActive = false;
volatile bool WinsockNetLayer::s_discovering = false;
pthread_mutex_t WinsockNetLayer::s_discoveryLock = PTHREAD_MUTEX_INITIALIZER;
std::vector<Win64LANSession> WinsockNetLayer::s_discoveredSessions;

pthread_mutex_t WinsockNetLayer::s_disconnectLock = PTHREAD_MUTEX_INITIALIZER;
std::vector<uint8_t> WinsockNetLayer::s_disconnectedSmallIds;

pthread_mutex_t WinsockNetLayer::s_pendingJoinLock = PTHREAD_MUTEX_INITIALIZER;
std::vector<uint8_t> WinsockNetLayer::s_pendingJoinSmallIds;

pthread_mutex_t WinsockNetLayer::s_freeSmallIdLock = PTHREAD_MUTEX_INITIALIZER;
std::vector<uint8_t> WinsockNetLayer::s_freeSmallIds;

pthread_mutex_t WinsockNetLayer::s_earlyDataLock = PTHREAD_MUTEX_INITIALIZER;
std::vector<std::vector<uint8_t> > WinsockNetLayer::s_earlyDataBuffers;

pthread_t WinsockNetLayer::s_asyncJoinThread;
bool WinsockNetLayer::s_asyncJoinThreadActive = false;
volatile WinsockNetLayer::JoinResult WinsockNetLayer::s_asyncJoinResult = WinsockNetLayer::JOIN_FAILED;
volatile bool WinsockNetLayer::s_asyncJoinActive = false;
char WinsockNetLayer::s_asyncJoinIP[256] = {};
int WinsockNetLayer::s_asyncJoinPort = 0;

bool g_Win64MultiplayerHost = false;
bool g_Win64MultiplayerJoin = false;
int g_Win64MultiplayerPort = WIN64_NET_DEFAULT_PORT;
char g_Win64MultiplayerIP[256] = "127.0.0.1";

bool g_ServerAdvertiseLAN = true;
char g_ServerBindAddress[256] = "";
int g_ServerMaxPlayers = MINECRAFT_NET_MAX_PLAYERS;

size_t WinsockNetLayer::GetConnectionSlotCount()
{
    int configured = g_ServerMaxPlayers;
    if (configured < 1)
        configured = 1;
    if (configured > 255)
        configured = 255;
    return (size_t)configured;
}

static void InitMutex(pthread_mutex_t* m)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
}

static bool JoinThreadWithTimeout(pthread_t thread, int timeoutMs, const char* threadName)
{
#if defined(__GLIBC__)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeoutMs / 1000;
    ts.tv_nsec += (timeoutMs % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    int rc = pthread_timedjoin_np(thread, NULL, &ts);
    if (rc == 0)
        return true;

    if (rc == ETIMEDOUT)
    {
        app.DebugPrintf("POSIX LAN: Timed out joining %s thread, cancelling\n", threadName);
        pthread_cancel(thread);

        struct timespec tsCancel;
        clock_gettime(CLOCK_REALTIME, &tsCancel);
        tsCancel.tv_sec += 1;
        rc = pthread_timedjoin_np(thread, NULL, &tsCancel);
        if (rc == 0)
            return true;
    }

    app.DebugPrintf("POSIX LAN: Failed to join %s thread (rc=%d), detaching\n", threadName, rc);
    pthread_detach(thread);
    return false;
#else
    (void)timeoutMs;
    (void)threadName;
    return pthread_join(thread, NULL) == 0;
#endif
}

bool WinsockNetLayer::Initialize()
{
    if (s_initialized) return true;


    InitMutex(&s_sendLock);
    InitMutex(&s_connectionsLock);
    InitMutex(&s_advertiseLock);
    InitMutex(&s_discoveryLock);
    InitMutex(&s_disconnectLock);
    InitMutex(&s_pendingJoinLock);
    InitMutex(&s_freeSmallIdLock);
    InitMutex(&s_earlyDataLock);

    size_t slotCount = GetConnectionSlotCount();
    s_connections.clear();
    s_connections.resize(slotCount);
    s_earlyDataBuffers.clear();
    s_earlyDataBuffers.resize(slotCount);

    for (size_t i = 0; i < slotCount; i++)
    {
        s_connections[i].tcpSocket = INVALID_SOCKET;
        s_connections[i].smallId = (uint8_t)i;
        s_connections[i].active = false;
        s_connections[i].recvBuffer = NULL;
        s_connections[i].recvBufferUsed = 0;
        s_connections[i].recvBufferSize = 0;
        s_connections[i].currentPacketSize = -1;
        s_connections[i].readingHeader = true;
        s_connections[i].sendBuffer = NULL;
        s_connections[i].sendBufferUsed = 0;
        s_connections[i].sendBufferSize = 0;
        InitMutex(&s_connections[i].sendLock);
        InitMutex(&s_connections[i].sendBufLock);
    }

    s_initialized = true;

#ifndef WITH_SERVER_CODE
    StartDiscovery();
#endif

    return true;
}

void WinsockNetLayer::Shutdown()
{
    CancelJoinGame();
    StopAdvertising();
    StopDiscovery();

    s_active = false;
    s_connected = false;

    if (s_listenSocket != INVALID_SOCKET)
    {
        shutdown(s_listenSocket, SHUT_RDWR);
        close(s_listenSocket);
        s_listenSocket = INVALID_SOCKET;
    }

    if (s_hostConnectionSocket != INVALID_SOCKET)
    {
        shutdown(s_hostConnectionSocket, SHUT_RDWR);
        close(s_hostConnectionSocket);
        s_hostConnectionSocket = INVALID_SOCKET;
    }

    if (s_epollFd >= 0)
    {
        close(s_epollFd);
        s_epollFd = -1;
    }

    if (s_epollThreadsActive)
    {
        for (int i = 0; i < WIN64_NET_IO_THREADS; i++)
            JoinThreadWithTimeout(s_epollThreads[i], 3000, "epoll");
        s_epollThreadsActive = false;
    }

    pthread_mutex_lock(&s_connectionsLock);
    for (size_t i = 0; i < s_connections.size(); i++)
    {
        s_connections[i].active = false;
        if (s_connections[i].tcpSocket != INVALID_SOCKET)
        {
            shutdown(s_connections[i].tcpSocket, SHUT_RDWR);
            close(s_connections[i].tcpSocket);
            s_connections[i].tcpSocket = INVALID_SOCKET;
        }
        if (s_connections[i].recvBuffer)
        {
            free(s_connections[i].recvBuffer);
            s_connections[i].recvBuffer = NULL;
        }
        if (s_connections[i].sendBuffer)
        {
            free(s_connections[i].sendBuffer);
            s_connections[i].sendBuffer = NULL;
        }
        pthread_mutex_destroy(&s_connections[i].sendLock);
        pthread_mutex_destroy(&s_connections[i].sendBufLock);
    }
    s_connections.clear();
    s_earlyDataBuffers.clear();
    pthread_mutex_unlock(&s_connectionsLock);

    if (s_acceptThreadActive)
    {
        JoinThreadWithTimeout(s_acceptThread, 3000, "accept");
        s_acceptThreadActive = false;
    }

    if (s_clientRecvThreadActive)
    {
        JoinThreadWithTimeout(s_clientRecvThread, 3000, "client-recv");
        s_clientRecvThreadActive = false;
    }

    if (s_initialized)
    {
        pthread_mutex_destroy(&s_sendLock);
        pthread_mutex_destroy(&s_connectionsLock);
        pthread_mutex_destroy(&s_advertiseLock);
        pthread_mutex_destroy(&s_discoveryLock);
        pthread_mutex_destroy(&s_disconnectLock);
        s_disconnectedSmallIds.clear();
        pthread_mutex_destroy(&s_pendingJoinLock);
        s_pendingJoinSmallIds.clear();
        pthread_mutex_destroy(&s_freeSmallIdLock);
        s_freeSmallIds.clear();
        s_initialized = false;
    }
}

bool WinsockNetLayer::HostGame(int port)
{
    if (!s_initialized && !Initialize()) return false;

    s_isHost = true;
    s_localSmallId = 0;
    s_hostSmallId = 0;
    s_nextSmallId = 1;
    s_hostGamePort = port;

    pthread_mutex_lock(&s_freeSmallIdLock);
    s_freeSmallIds.clear();
    pthread_mutex_unlock(&s_freeSmallIdLock);

    s_listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_listenSocket == INVALID_SOCKET)
    {
        app.DebugPrintf("socket() failed: %s", strerror(errno));
        return false;
    }

    int opt = 1;
    ::setsockopt(s_listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    extern char g_ServerBindAddress[256];
    if (g_ServerBindAddress[0] != '\0')
        addr.sin_addr.s_addr = inet_addr(g_ServerBindAddress);
    else
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iResult = ::bind(s_listenSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (iResult < 0)
    {
        app.DebugPrintf("bind() failed: %s", strerror(errno));
        ::close(s_listenSocket);
        s_listenSocket = INVALID_SOCKET;
        return false;
    }

    iResult = ::listen(s_listenSocket, SOMAXCONN);
    if (iResult < 0)
    {
        app.DebugPrintf("listen() failed: %s", strerror(errno));
        ::close(s_listenSocket);
        s_listenSocket = INVALID_SOCKET;
        return false;
    }

    s_active = true;
    s_connected = true;

    s_epollFd = epoll_create1(0);
    if (s_epollFd < 0)
    {
        app.DebugPrintf("epoll_create1() failed: %s", strerror(errno));
        ::close(s_listenSocket);
        s_listenSocket = INVALID_SOCKET;
        return false;
    }

    s_epollThreadsActive = true;
    for (int i = 0; i < WIN64_NET_IO_THREADS; i++)
        pthread_create(&s_epollThreads[i], NULL, EpollThreadProc, NULL);

    if (pthread_create(&s_acceptThread, NULL, AcceptThreadProc, NULL) == 0)
        s_acceptThreadActive = true;

    app.DebugPrintf("POSIX LAN: Hosting on port %d", port);
    return true;
}

bool WinsockNetLayer::JoinGame(const char *ip, int port)
{
    if (!s_initialized && !Initialize()) return false;

    s_isHost = false;
    s_hostSmallId = 0;
    s_connected = false;
    s_active = false;

    if (s_hostConnectionSocket != INVALID_SOCKET)
    {
        close(s_hostConnectionSocket);
        s_hostConnectionSocket = INVALID_SOCKET;
    }

    struct addrinfo hints = {};
    struct addrinfo *result = NULL;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char portStr[16];
    snprintf(portStr, sizeof(portStr), "%d", port);

    int iResult = getaddrinfo(ip, portStr, &hints, &result);
    if (iResult != 0)
    {
        app.DebugPrintf("getaddrinfo failed for %s:%d - %s\n", ip, port, gai_strerror(iResult));
        return false;
    }

    bool connected = false;
    uint8_t assignedSmallId = 0;
    const int maxAttempts = 3;
    const int connectTimeoutMs = 3000;

    for (int attempt = 0; attempt < maxAttempts; ++attempt)
    {
        s_hostConnectionSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (s_hostConnectionSocket == INVALID_SOCKET)
        {
            app.DebugPrintf("socket() failed: %s\n", strerror(errno));
            break;
        }

        int noDelay = 1;
        setsockopt(s_hostConnectionSocket, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));

        int flags = fcntl(s_hostConnectionSocket, F_GETFL, 0);
        fcntl(s_hostConnectionSocket, F_SETFL, flags | O_NONBLOCK);

        iResult = connect(s_hostConnectionSocket, result->ai_addr, result->ai_addrlen);
        if (iResult < 0 && errno == EINPROGRESS)
        {
            struct pollfd pfd;
            pfd.fd = s_hostConnectionSocket;
            pfd.events = POLLOUT;
            pfd.revents = 0;

            int pollResult = poll(&pfd, 1, connectTimeoutMs);
            if (pollResult <= 0 || (pfd.revents & (POLLERR | POLLHUP)))
            {
                app.DebugPrintf("connect() to %s:%d timed out or failed (attempt %d/%d)\n", ip, port, attempt + 1, maxAttempts);
                close(s_hostConnectionSocket);
                s_hostConnectionSocket = INVALID_SOCKET;
                continue;
            }

            int sockErr = 0;
            socklen_t sockErrLen = sizeof(sockErr);
            getsockopt(s_hostConnectionSocket, SOL_SOCKET, SO_ERROR, &sockErr, &sockErrLen);
            if (sockErr != 0)
            {
                app.DebugPrintf("connect() to %s:%d failed with SO_ERROR %d (attempt %d/%d)\n", ip, port, sockErr, attempt + 1, maxAttempts);
                close(s_hostConnectionSocket);
                s_hostConnectionSocket = INVALID_SOCKET;
                continue;
            }
        }
        else if (iResult < 0)
        {
            app.DebugPrintf("connect() to %s:%d failed (attempt %d/%d): %s\n", ip, port, attempt + 1, maxAttempts, strerror(errno));
            close(s_hostConnectionSocket);
            s_hostConnectionSocket = INVALID_SOCKET;
            continue;
        }

        fcntl(s_hostConnectionSocket, F_SETFL, flags & ~O_NONBLOCK);

        struct timeval recvTimeout;
        recvTimeout.tv_sec = 3;
        recvTimeout.tv_usec = 0;
        setsockopt(s_hostConnectionSocket, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout));

        uint8_t assignBuf[1];
        ssize_t bytesRecv = recv(s_hostConnectionSocket, (char *)assignBuf, 1, 0);
        if (bytesRecv != 1)
        {
            app.DebugPrintf("Failed to receive small ID assignment from host (attempt %d/%d)\n", attempt + 1, maxAttempts);
            close(s_hostConnectionSocket);
            s_hostConnectionSocket = INVALID_SOCKET;
            continue;
        }

        assignedSmallId = assignBuf[0];
        connected = true;
        break;
    }
    freeaddrinfo(result);

    if (!connected)
    {
        return false;
    }
    s_localSmallId = assignedSmallId;

    struct timeval noTimeout = {0, 0};
    setsockopt(s_hostConnectionSocket, SOL_SOCKET, SO_RCVTIMEO, &noTimeout, sizeof(noTimeout));

    app.DebugPrintf("POSIX LAN: Connected to %s:%d, assigned smallId=%d\n", ip, port, s_localSmallId);

    s_active = true;
    s_connected = true;

    if (pthread_create(&s_clientRecvThread, NULL, ClientRecvThreadProc, NULL) == 0)
        s_clientRecvThreadActive = true;

    return true;
}

bool WinsockNetLayer::BeginJoinGame(const char *ip, int port)
{
    if (s_asyncJoinActive)
        return false;

    CancelJoinGame();

    strncpy(s_asyncJoinIP, ip, sizeof(s_asyncJoinIP) - 1);
    s_asyncJoinIP[sizeof(s_asyncJoinIP) - 1] = '\0';
    s_asyncJoinPort = port;
    s_asyncJoinResult = JOIN_IN_PROGRESS;
    s_asyncJoinActive = true;

    if (pthread_create(&s_asyncJoinThread, NULL, AsyncJoinThreadProc, NULL) == 0)
    {
        s_asyncJoinThreadActive = true;
        return true;
    }

    s_asyncJoinActive = false;
    s_asyncJoinResult = JOIN_FAILED;
    return false;
}

WinsockNetLayer::JoinResult WinsockNetLayer::PollJoinResult()
{
    return s_asyncJoinResult;
}

void WinsockNetLayer::CancelJoinGame()
{
    if (s_hostConnectionSocket != INVALID_SOCKET && !s_connected)
    {
        shutdown(s_hostConnectionSocket, SHUT_RDWR);
        close(s_hostConnectionSocket);
        s_hostConnectionSocket = INVALID_SOCKET;
    }

    if (s_asyncJoinThreadActive)
    {
        s_asyncJoinActive = false;
        JoinThreadWithTimeout(s_asyncJoinThread, 3000, "async-join");
        s_asyncJoinThreadActive = false;
    }
}

void* WinsockNetLayer::AsyncJoinThreadProc(void* /*param*/)
{
    bool result = JoinGame(s_asyncJoinIP, s_asyncJoinPort);
    s_asyncJoinResult = result ? JOIN_SUCCESS : JOIN_FAILED;
    s_asyncJoinActive = false;
    return NULL;
}

bool WinsockNetLayer::SendOnSocket(SOCKET sock, const void *data, int dataSize)
{
    if (sock == INVALID_SOCKET || dataSize <= 0 || dataSize > WIN64_NET_MAX_PACKET_SIZE) return false;

    uint8_t header[4];
    header[0] = (uint8_t)((dataSize >> 24) & 0xFF);
    header[1] = (uint8_t)((dataSize >> 16) & 0xFF);
    header[2] = (uint8_t)((dataSize >> 8) & 0xFF);
    header[3] = (uint8_t)(dataSize & 0xFF);

    int totalSent = 0;
    int toSend = 4;
    while (totalSent < toSend)
    {
        ssize_t sent = send(sock, (const char *)header + totalSent, toSend - totalSent, MSG_NOSIGNAL);
        if (sent <= 0) return false;
        totalSent += sent;
    }

    totalSent = 0;
    while (totalSent < dataSize)
    {
        ssize_t sent = send(sock, (const char *)data + totalSent, dataSize - totalSent, MSG_NOSIGNAL);
        if (sent <= 0) return false;
        totalSent += sent;
    }

    return true;
}

bool WinsockNetLayer::SendToSmallId(uint8_t targetSmallId, const void *data, int dataSize)
{
    if (!s_active) return false;

    if (s_isHost)
    {
        if ((size_t)targetSmallId >= s_connections.size() || !s_connections[targetSmallId].active)
            return false;

        uint8_t header[4];
        header[0] = (uint8_t)((dataSize >> 24) & 0xFF);
        header[1] = (uint8_t)((dataSize >> 16) & 0xFF);
        header[2] = (uint8_t)((dataSize >> 8) & 0xFF);
        header[3] = (uint8_t)(dataSize & 0xFF);

        Win64RemoteConnection &conn = s_connections[targetSmallId];
        pthread_mutex_lock(&conn.sendBufLock);

        int totalNeeded = conn.sendBufferUsed + 4 + dataSize;
        if (totalNeeded > conn.sendBufferSize)
        {
            int newSize = conn.sendBufferSize;
            while (newSize < totalNeeded)
                newSize *= 2;
            if (newSize > WIN64_NET_MAX_PACKET_SIZE * 2)
                newSize = WIN64_NET_MAX_PACKET_SIZE * 2;
            if (totalNeeded > newSize)
            {
                pthread_mutex_unlock(&conn.sendBufLock);
                return false;
            }
            uint8_t *newBuf = (uint8_t *)realloc(conn.sendBuffer, newSize);
            if (!newBuf)
            {
                pthread_mutex_unlock(&conn.sendBufLock);
                return false;
            }
            conn.sendBuffer = newBuf;
            conn.sendBufferSize = newSize;
        }

        memcpy(conn.sendBuffer + conn.sendBufferUsed, header, 4);
        conn.sendBufferUsed += 4;
        memcpy(conn.sendBuffer + conn.sendBufferUsed, data, dataSize);
        conn.sendBufferUsed += dataSize;

        pthread_mutex_unlock(&conn.sendBufLock);
        return true;
    }
    else
    {
        pthread_mutex_lock(&s_sendLock);
        bool result = SendOnSocket(s_hostConnectionSocket, data, dataSize);
        pthread_mutex_unlock(&s_sendLock);
        return result;
    }
}

SOCKET WinsockNetLayer::GetSocketForSmallId(uint8_t smallId)
{
    pthread_mutex_lock(&s_connectionsLock);
    if ((size_t)smallId < s_connections.size() && s_connections[smallId].active)
    {
        SOCKET sock = s_connections[smallId].tcpSocket;
        pthread_mutex_unlock(&s_connectionsLock);
        return sock;
    }
    pthread_mutex_unlock(&s_connectionsLock);
    return INVALID_SOCKET;
}

std::string WinsockNetLayer::GetIPForSmallId(uint8_t smallId)
{
    SOCKET sock = GetSocketForSmallId(smallId);
    if (sock == INVALID_SOCKET) return "";
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    if (getpeername(sock, (struct sockaddr *)&addr, &addrLen) != 0) return "";
    char buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)) == NULL) return "";
    return std::string(buf);
}

static bool RecvExact(SOCKET sock, uint8_t *buf, int len)
{
    int totalRecv = 0;
    while (totalRecv < len)
    {
        ssize_t r = recv(sock, (char *)buf + totalRecv, len - totalRecv, 0);
        if (r <= 0) return false;
        totalRecv += r;
    }
    return true;
}

void WinsockNetLayer::HandleDataReceived(uint8_t fromSmallId, uint8_t toSmallId, unsigned char *data, unsigned int dataSize)
{
    INetworkPlayer *pPlayerFrom = g_NetworkManager.GetPlayerBySmallId(fromSmallId);
    INetworkPlayer *pPlayerTo = g_NetworkManager.GetPlayerBySmallId(toSmallId);

    if (pPlayerFrom == NULL || pPlayerTo == NULL)
    {
        if (s_isHost && fromSmallId > 0 && (size_t)fromSmallId < s_earlyDataBuffers.size())
        {
            pthread_mutex_lock(&s_earlyDataLock);
            s_earlyDataBuffers[fromSmallId].insert(
                s_earlyDataBuffers[fromSmallId].end(), data, data + dataSize);
            pthread_mutex_unlock(&s_earlyDataLock);
        }
        return;
    }

    if (s_isHost)
    {
        ::Socket *pSocket = pPlayerFrom->GetSocket();
        if (pSocket != NULL)
            pSocket->pushDataToQueue(data, dataSize, false);
        else
        {
            pthread_mutex_lock(&s_earlyDataLock);
            s_earlyDataBuffers[fromSmallId].insert(
                s_earlyDataBuffers[fromSmallId].end(), data, data + dataSize);
            pthread_mutex_unlock(&s_earlyDataLock);
        }
    }
    else
    {
        ::Socket *pSocket = pPlayerTo->GetSocket();
        if (pSocket != NULL)
            pSocket->pushDataToQueue(data, dataSize, true);
    }
}

void WinsockNetLayer::FlushPendingData()
{
    pthread_mutex_lock(&s_earlyDataLock);
    for (size_t i = 1; i < s_earlyDataBuffers.size(); i++)
    {
        if (s_earlyDataBuffers[i].empty()) continue;

        INetworkPlayer *pPlayer = g_NetworkManager.GetPlayerBySmallId((uint8_t)i);
        if (pPlayer == NULL) continue;

        ::Socket *pSocket = pPlayer->GetSocket();
        if (pSocket == NULL) continue;

        pSocket->pushDataToQueue(s_earlyDataBuffers[i].data(),
            (DWORD)s_earlyDataBuffers[i].size(), false);
        s_earlyDataBuffers[i].clear();
    }
    pthread_mutex_unlock(&s_earlyDataLock);
}

void* WinsockNetLayer::AcceptThreadProc(void* /*param*/)
{
    while (s_active)
    {
        SOCKET clientSocket = accept(s_listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            if (s_active)
                app.DebugPrintf("accept() failed: %s\n", strerror(errno));
            break;
        }

        int noDelay = 1;
        setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));

        int sndbuf = WIN64_NET_SEND_BUFFER_SIZE;
        setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
        int rcvbuf = WIN64_NET_RECV_BUFFER_SIZE;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

        extern QNET_STATE _iQNetStubState;
        if (_iQNetStubState != QNET_STATE_GAME_PLAY)
        {
            app.DebugPrintf("POSIX LAN: Rejecting connection, game not ready\n");
            close(clientSocket);
            continue;
        }

        uint8_t assignedSmallId;
        pthread_mutex_lock(&s_freeSmallIdLock);
        if (!s_freeSmallIds.empty())
        {
            assignedSmallId = s_freeSmallIds.back();
            s_freeSmallIds.pop_back();
        }
        else
        {
            uint8_t expected = s_nextSmallId.load();
            if (expected < g_ServerMaxPlayers)
            {
                assignedSmallId = expected;
                s_nextSmallId.store(expected + 1);
            }
            else
            {
                pthread_mutex_unlock(&s_freeSmallIdLock);
                app.DebugPrintf("POSIX LAN: Server full, rejecting connection\n");
                close(clientSocket);
                continue;
            }
        }
        pthread_mutex_unlock(&s_freeSmallIdLock);

        if (assignedSmallId >= IQNet::GetPlayerCapacity())
        {
            app.DebugPrintf("POSIX LAN: Invalid smallId %d for IQNet capacity, rejecting\n", assignedSmallId);
            close(clientSocket);
            continue;
        }

        if ((size_t)assignedSmallId >= s_connections.size())
        {
            app.DebugPrintf("POSIX LAN: Invalid smallId %d for transport capacity, rejecting\n", assignedSmallId);
            close(clientSocket);
            continue;
        }

        uint8_t assignBuf[1] = { assignedSmallId };
        ssize_t sent = send(clientSocket, (const char *)assignBuf, 1, MSG_NOSIGNAL);
        if (sent != 1)
        {
            app.DebugPrintf("Failed to send small ID to client\n");
            close(clientSocket);
            continue;
        }

        int flags = fcntl(clientSocket, F_GETFL, 0);
        fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

        Win64RemoteConnection &conn = s_connections[assignedSmallId];

        pthread_mutex_lock(&s_connectionsLock);
        if (conn.recvBuffer)
            free(conn.recvBuffer);
        if (conn.sendBuffer)
            free(conn.sendBuffer);
        conn.tcpSocket = clientSocket;
        conn.smallId = assignedSmallId;
        conn.active = true;
        conn.recvBuffer = (uint8_t *)malloc(WIN64_NET_RECV_BUFFER_SIZE);
        conn.recvBufferUsed = 0;
        conn.recvBufferSize = WIN64_NET_RECV_BUFFER_SIZE;
        conn.currentPacketSize = -1;
        conn.readingHeader = true;
        conn.sendBuffer = (uint8_t *)malloc(WIN64_NET_SEND_BUFFER_SIZE);
        conn.sendBufferUsed = 0;
        conn.sendBufferSize = WIN64_NET_SEND_BUFFER_SIZE;
        pthread_mutex_unlock(&s_connectionsLock);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.u32 = assignedSmallId;
        epoll_ctl(s_epollFd, EPOLL_CTL_ADD, clientSocket, &ev);

        app.DebugPrintf("POSIX LAN: Client connected, assigned smallId=%d (epoll)\n", assignedSmallId);

        IQNetPlayer *qnetPlayer = &IQNet::m_player[assignedSmallId];

        extern void Win64_SetupRemoteQNetPlayer(IQNetPlayer *player, uint8_t smallId, bool isHost, bool isLocal);
        Win64_SetupRemoteQNetPlayer(qnetPlayer, assignedSmallId, false, false);

        pthread_mutex_lock(&s_pendingJoinLock);
        s_pendingJoinSmallIds.push_back(assignedSmallId);
        pthread_mutex_unlock(&s_pendingJoinLock);
    }
    return NULL;
}

bool WinsockNetLayer::ProcessRecvData(Win64RemoteConnection &conn)
{
    while (conn.recvBufferUsed > 0)
    {
        if (conn.readingHeader)
        {
            if (conn.recvBufferUsed < 4)
                return true;

            int packetSize =
                ((uint32_t)conn.recvBuffer[0] << 24) |
                ((uint32_t)conn.recvBuffer[1] << 16) |
                ((uint32_t)conn.recvBuffer[2] << 8) |
                ((uint32_t)conn.recvBuffer[3]);

            if (packetSize <= 0 || (unsigned int)packetSize > WIN64_NET_MAX_PACKET_SIZE)
                return false;

            conn.currentPacketSize = packetSize;
            conn.readingHeader = false;

            memmove(conn.recvBuffer, conn.recvBuffer + 4, conn.recvBufferUsed - 4);
            conn.recvBufferUsed -= 4;
        }
        else
        {
            if (conn.recvBufferUsed < conn.currentPacketSize)
                return true;

            HandleDataReceived(conn.smallId, s_hostSmallId, conn.recvBuffer, conn.currentPacketSize);

            int remaining = conn.recvBufferUsed - conn.currentPacketSize;
            if (remaining > 0)
                memmove(conn.recvBuffer, conn.recvBuffer + conn.currentPacketSize, remaining);
            conn.recvBufferUsed = remaining;
            conn.currentPacketSize = -1;
            conn.readingHeader = true;
        }
    }
    return true;
}

void* WinsockNetLayer::EpollThreadProc(void* /*param*/)
{
    struct epoll_event events[WIN64_NET_EPOLL_MAX_EVENTS];

    while (s_active)
    {
        int nfds = epoll_wait(s_epollFd, events, WIN64_NET_EPOLL_MAX_EVENTS, 50);
        if (nfds < 0)
        {
            if (errno == EINTR) continue;
            break;
        }

        for (int i = 0; i < nfds; i++)
        {
            uint8_t smallId = (uint8_t)events[i].data.u32;

            if ((size_t)smallId >= s_connections.size())
                continue;

            Win64RemoteConnection &conn = s_connections[smallId];
            if (!conn.active)
                continue;

            if (events[i].events & (EPOLLERR | EPOLLHUP))
            {
                conn.active = false;
                epoll_ctl(s_epollFd, EPOLL_CTL_DEL, conn.tcpSocket, NULL);
                close(conn.tcpSocket);
                conn.tcpSocket = INVALID_SOCKET;

                pthread_mutex_lock(&s_disconnectLock);
                s_disconnectedSmallIds.push_back(smallId);
                pthread_mutex_unlock(&s_disconnectLock);
                continue;
            }

            if (events[i].events & EPOLLIN)
            {
                bool disconnected = false;
                while (true)
                {
                    int space = conn.recvBufferSize - conn.recvBufferUsed;
                    if (space < 4096)
                    {
                        int newSize = conn.recvBufferSize * 2;
                        if (newSize > WIN64_NET_MAX_PACKET_SIZE + 4)
                            newSize = WIN64_NET_MAX_PACKET_SIZE + 4;
                        uint8_t *newBuf = (uint8_t *)realloc(conn.recvBuffer, newSize);
                        if (!newBuf)
                        {
                            disconnected = true;
                            break;
                        }
                        conn.recvBuffer = newBuf;
                        conn.recvBufferSize = newSize;
                        space = newSize - conn.recvBufferUsed;
                    }

                    ssize_t r = recv(conn.tcpSocket, (char *)conn.recvBuffer + conn.recvBufferUsed, space, 0);
                    if (r > 0)
                    {
                        conn.recvBufferUsed += (int)r;
                        if (!ProcessRecvData(conn))
                        {
                            disconnected = true;
                            break;
                        }
                    }
                    else if (r == 0)
                    {
                        disconnected = true;
                        break;
                    }
                    else
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        disconnected = true;
                        break;
                    }
                }

                if (disconnected)
                {
                    conn.active = false;
                    epoll_ctl(s_epollFd, EPOLL_CTL_DEL, conn.tcpSocket, NULL);
                    close(conn.tcpSocket);
                    conn.tcpSocket = INVALID_SOCKET;

                    pthread_mutex_lock(&s_disconnectLock);
                    s_disconnectedSmallIds.push_back(smallId);
                    pthread_mutex_unlock(&s_disconnectLock);
                }
            }
        }
    }
    return NULL;
}

void WinsockNetLayer::FlushSendBuffers()
{
    pthread_mutex_lock(&s_connectionsLock);
    for (size_t i = 1; i < s_connections.size(); i++)
    {
        Win64RemoteConnection &conn = s_connections[i];
        if (!conn.active || conn.tcpSocket == INVALID_SOCKET)
            continue;

        pthread_mutex_lock(&conn.sendBufLock);
            if (conn.sendBufferUsed > 0)
        {
            int totalSent = 0;
            while (totalSent < conn.sendBufferUsed)
            {
                ssize_t sent = send(conn.tcpSocket, (const char *)conn.sendBuffer + totalSent,
                    conn.sendBufferUsed - totalSent, MSG_NOSIGNAL);
                if (sent <= 0)
                    break;
                totalSent += (int)sent;
            }
            if (totalSent < conn.sendBufferUsed && totalSent > 0)
            {
                memmove(conn.sendBuffer, conn.sendBuffer + totalSent, conn.sendBufferUsed - totalSent);
                conn.sendBufferUsed -= totalSent;
            }
            else
            {
                conn.sendBufferUsed = 0;
            }
        }
        pthread_mutex_unlock(&conn.sendBufLock);
    }
    pthread_mutex_unlock(&s_connectionsLock);
}

bool WinsockNetLayer::PopDisconnectedSmallId(uint8_t *outSmallId)
{
    bool found = false;
    pthread_mutex_lock(&s_disconnectLock);
    if (!s_disconnectedSmallIds.empty())
    {
        *outSmallId = s_disconnectedSmallIds.back();
        s_disconnectedSmallIds.pop_back();
        found = true;
    }
    pthread_mutex_unlock(&s_disconnectLock);
    return found;
}

void WinsockNetLayer::PushFreeSmallId(uint8_t smallId)
{
    pthread_mutex_lock(&s_freeSmallIdLock);
    s_freeSmallIds.push_back(smallId);
    pthread_mutex_unlock(&s_freeSmallIdLock);
}

bool WinsockNetLayer::PopPendingJoinSmallId(uint8_t *outSmallId)
{
    bool found = false;
    pthread_mutex_lock(&s_pendingJoinLock);
    if (!s_pendingJoinSmallIds.empty())
    {
        *outSmallId = s_pendingJoinSmallIds.back();
        s_pendingJoinSmallIds.pop_back();
        found = true;
    }
    pthread_mutex_unlock(&s_pendingJoinLock);
    return found;
}

bool WinsockNetLayer::IsSmallIdConnected(uint8_t smallId)
{
    if ((size_t)smallId >= s_connections.size()) return false;
    return s_connections[smallId].active;
}

void WinsockNetLayer::CloseConnectionBySmallId(uint8_t smallId)
{
    pthread_mutex_lock(&s_connectionsLock);
    if ((size_t)smallId < s_connections.size() && s_connections[smallId].active && s_connections[smallId].tcpSocket != INVALID_SOCKET)
    {
        epoll_ctl(s_epollFd, EPOLL_CTL_DEL, s_connections[smallId].tcpSocket, NULL);
        shutdown(s_connections[smallId].tcpSocket, SHUT_RDWR);
        close(s_connections[smallId].tcpSocket);
        s_connections[smallId].tcpSocket = INVALID_SOCKET;
        app.DebugPrintf("POSIX LAN: Force-closed TCP connection for smallId=%d\n", smallId);
    }
    pthread_mutex_unlock(&s_connectionsLock);

    pthread_mutex_lock(&s_earlyDataLock);
    if ((size_t)smallId < s_earlyDataBuffers.size())
        s_earlyDataBuffers[smallId].clear();
    pthread_mutex_unlock(&s_earlyDataLock);
}

void* WinsockNetLayer::ClientRecvThreadProc(void* /*param*/)
{
    std::vector<uint8_t> recvBuf;
    recvBuf.resize(WIN64_NET_RECV_BUFFER_SIZE);

    while (s_active && s_hostConnectionSocket != INVALID_SOCKET)
    {
        uint8_t header[4];
        if (!RecvExact(s_hostConnectionSocket, header, 4))
        {
            app.DebugPrintf("POSIX LAN: Disconnected from host (header)\n");
            break;
        }

        int packetSize = ((uint32_t)header[0] << 24) | ((uint32_t)header[1] << 16) | ((uint32_t)header[2] << 8) | (uint32_t)header[3];

        if (packetSize <= 0 || (unsigned int)packetSize > WIN64_NET_MAX_PACKET_SIZE)
        {
            app.DebugPrintf("POSIX LAN: Invalid packet size %d from host (max=%d)\n",
                packetSize, (int)WIN64_NET_MAX_PACKET_SIZE);
            break;
        }

        if ((int)recvBuf.size() < packetSize)
            recvBuf.resize(packetSize);

        if (!RecvExact(s_hostConnectionSocket, &recvBuf[0], packetSize))
        {
            app.DebugPrintf("POSIX LAN: Disconnected from host (body)\n");
            break;
        }

        HandleDataReceived(s_hostSmallId, s_localSmallId, &recvBuf[0], packetSize);
    }

    s_connected = false;
    return NULL;
}

bool WinsockNetLayer::StartAdvertising(int gamePort, const wchar_t *hostName, unsigned int gameSettings, unsigned int texPackId, unsigned char subTexId, unsigned short netVer)
{
    if (s_advertising) return true;
    if (!s_initialized) return false;

    pthread_mutex_lock(&s_advertiseLock);
    memset(&s_advertiseData, 0, sizeof(s_advertiseData));
    s_advertiseData.magic = WIN64_LAN_BROADCAST_MAGIC;
    s_advertiseData.netVersion = netVer;
    s_advertiseData.gamePort = (uint16_t)gamePort;
    for (int i = 0; i < 31 && hostName[i] != L'\0'; i++)
        s_advertiseData.hostName[i] = (uint16_t)hostName[i];
    s_advertiseData.playerCount = 1;
    s_advertiseData.maxPlayers = g_ServerMaxPlayers;
    s_advertiseData.gameHostSettings = gameSettings;
    s_advertiseData.texturePackParentId = texPackId;
    s_advertiseData.subTexturePackId = subTexId;
    s_advertiseData.isJoinable = 0;
#ifdef WITH_SERVER_CODE
    s_advertiseData.isDedicatedServer = 1;
#else
    s_advertiseData.isDedicatedServer = 0;
#endif
    s_hostGamePort = gamePort;
    pthread_mutex_unlock(&s_advertiseLock);

    s_advertiseSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_advertiseSock == INVALID_SOCKET)
    {
        app.DebugPrintf("POSIX LAN: Failed to create advertise socket: %s\n", strerror(errno));
        return false;
    }

    int broadcast = 1;
    setsockopt(s_advertiseSock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    s_advertising = true;
    if (pthread_create(&s_advertiseThread, NULL, AdvertiseThreadProc, NULL) == 0)
        s_advertiseThreadActive = true;

    app.DebugPrintf("POSIX LAN: Started advertising on UDP port %d\n", gamePort);
    return true;
}

void WinsockNetLayer::StopAdvertising()
{
    s_advertising = false;

    if (s_advertiseSock != INVALID_SOCKET)
    {
        shutdown(s_advertiseSock, SHUT_RDWR);
        close(s_advertiseSock);
        s_advertiseSock = INVALID_SOCKET;
    }

    if (s_advertiseThreadActive)
    {
        JoinThreadWithTimeout(s_advertiseThread, 2000, "advertise");
        s_advertiseThreadActive = false;
    }
}

void WinsockNetLayer::UpdateAdvertisePlayerCount(uint8_t count)
{
    pthread_mutex_lock(&s_advertiseLock);
    s_advertiseData.playerCount = count;
    pthread_mutex_unlock(&s_advertiseLock);
}

void WinsockNetLayer::UpdateAdvertisePlayerNames(uint8_t count, const char playerNames[][32])
{
    pthread_mutex_lock(&s_advertiseLock);
    memset(s_advertiseData.playerNames, 0, sizeof(s_advertiseData.playerNames));
    s_advertiseData.playerCount = count;
    for (int i = 0; i < count && i < WIN64_LAN_BROADCAST_PLAYERS; i++)
        memcpy(s_advertiseData.playerNames[i], playerNames[i], 32);
    pthread_mutex_unlock(&s_advertiseLock);
}

void WinsockNetLayer::UpdateAdvertiseJoinable(bool joinable)
{
    pthread_mutex_lock(&s_advertiseLock);
    s_advertiseData.isJoinable = joinable ? 1 : 0;
    pthread_mutex_unlock(&s_advertiseLock);
}

void WinsockNetLayer::UpdateAdvertiseGameHostSettings(unsigned int settings)
{
    pthread_mutex_lock(&s_advertiseLock);
    s_advertiseData.gameHostSettings = settings;
    pthread_mutex_unlock(&s_advertiseLock);
}

void* WinsockNetLayer::AdvertiseThreadProc(void* /*param*/)
{
    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    while (s_advertising)
    {
        pthread_mutex_lock(&s_advertiseLock);
        Win64LANBroadcast data = s_advertiseData;
        int gamePort = s_hostGamePort;
        pthread_mutex_unlock(&s_advertiseLock);

        broadcastAddr.sin_port = htons((uint16_t)gamePort);
        ssize_t sent = sendto(s_advertiseSock, (const char *)&data, sizeof(data), 0,
            (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));

        if (sent < 0 && s_advertising)
            app.DebugPrintf("POSIX LAN: Broadcast sendto failed: %s\n", strerror(errno));

        if (gamePort != WIN64_LAN_DISCOVERY_PORT)
        {
            broadcastAddr.sin_port = htons(WIN64_LAN_DISCOVERY_PORT);
            sendto(s_advertiseSock, (const char *)&data, sizeof(data), 0,
                (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
        }

        sleep(1);
    }

    return NULL;
}

bool WinsockNetLayer::StartDiscovery()
{
    if (s_discovering) return true;
    if (!s_initialized) return false;

    int reuseAddr = 1;
    struct sockaddr_in bindAddr;
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = INADDR_ANY;

    s_discoverySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_discoverySock == INVALID_SOCKET)
    {
        app.DebugPrintf("POSIX LAN: Failed to create discovery socket: %s\n", strerror(errno));
    }
    else
    {
        setsockopt(s_discoverySock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
        bindAddr.sin_port = htons(WIN64_NET_DEFAULT_PORT);
        if (bind(s_discoverySock, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) < 0)
        {
            app.DebugPrintf("POSIX LAN: Discovery bind to port %d failed: %s\n", WIN64_NET_DEFAULT_PORT, strerror(errno));
            close(s_discoverySock);
            s_discoverySock = INVALID_SOCKET;
        }
        else
        {
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;
            setsockopt(s_discoverySock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        }
    }

    s_discoveryLegacySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_discoveryLegacySock != INVALID_SOCKET)
    {
        setsockopt(s_discoveryLegacySock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
        bindAddr.sin_port = htons(WIN64_LAN_DISCOVERY_PORT);
        if (bind(s_discoveryLegacySock, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) < 0)
        {
            app.DebugPrintf("POSIX LAN: Legacy discovery bind to port %d failed: %s\n", WIN64_LAN_DISCOVERY_PORT, strerror(errno));
            close(s_discoveryLegacySock);
            s_discoveryLegacySock = INVALID_SOCKET;
        }
        else
        {
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;
            setsockopt(s_discoveryLegacySock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        }
    }

    if (s_discoverySock == INVALID_SOCKET && s_discoveryLegacySock == INVALID_SOCKET)
        return false;

    s_discovering = true;
    if (pthread_create(&s_discoveryThread, NULL, DiscoveryThreadProc, NULL) == 0)
        s_discoveryThreadActive = true;

    app.DebugPrintf("POSIX LAN: Listening for LAN games on UDP ports %d and %d\n", WIN64_NET_DEFAULT_PORT, WIN64_LAN_DISCOVERY_PORT);
    return true;
}

void WinsockNetLayer::StopDiscovery()
{
    s_discovering = false;

    if (s_discoverySock != INVALID_SOCKET)
    {
        shutdown(s_discoverySock, SHUT_RDWR);
        close(s_discoverySock);
        s_discoverySock = INVALID_SOCKET;
    }

    if (s_discoveryLegacySock != INVALID_SOCKET)
    {
        shutdown(s_discoveryLegacySock, SHUT_RDWR);
        close(s_discoveryLegacySock);
        s_discoveryLegacySock = INVALID_SOCKET;
    }

    if (s_discoveryThreadActive)
    {
        JoinThreadWithTimeout(s_discoveryThread, 2000, "discovery");
        s_discoveryThreadActive = false;
    }

    pthread_mutex_lock(&s_discoveryLock);
    s_discoveredSessions.clear();
    pthread_mutex_unlock(&s_discoveryLock);
}

std::vector<Win64LANSession> WinsockNetLayer::GetDiscoveredSessions()
{
    std::vector<Win64LANSession> result;
    pthread_mutex_lock(&s_discoveryLock);
    result = s_discoveredSessions;
    pthread_mutex_unlock(&s_discoveryLock);
    return result;
}

void* WinsockNetLayer::DiscoveryThreadProc(void* /*param*/)
{
    char recvBuf[1024];
    const size_t MAX_DISCOVERED_SESSIONS = 64;

    while (s_discovering)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        int nfds = 0;
        if (s_discoverySock != INVALID_SOCKET)
        {
            FD_SET(s_discoverySock, &readSet);
            if (s_discoverySock > nfds) nfds = s_discoverySock;
        }
        if (s_discoveryLegacySock != INVALID_SOCKET)
        {
            FD_SET(s_discoveryLegacySock, &readSet);
            if (s_discoveryLegacySock > nfds) nfds = s_discoveryLegacySock;
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        int selResult = select(nfds + 1, &readSet, NULL, NULL, &tv);
        if (selResult <= 0)
            continue;

        int readySocks[2];
        int readyCount = 0;
        if (s_discoverySock != INVALID_SOCKET && FD_ISSET(s_discoverySock, &readSet))
            readySocks[readyCount++] = s_discoverySock;
        if (s_discoveryLegacySock != INVALID_SOCKET && FD_ISSET(s_discoveryLegacySock, &readSet))
            readySocks[readyCount++] = s_discoveryLegacySock;

        for (int si = 0; si < readyCount; si++)
        {
            struct sockaddr_in senderAddr;
            socklen_t senderLen = sizeof(senderAddr);

            ssize_t recvLen = recvfrom(readySocks[si], recvBuf, sizeof(recvBuf), 0,
                (struct sockaddr *)&senderAddr, &senderLen);

            if (recvLen < 0)
                continue;

            if ((size_t)recvLen < sizeof(Win64LANBroadcast))
                continue;

            Win64LANBroadcast *broadcast = (Win64LANBroadcast *)recvBuf;
            if (broadcast->magic != WIN64_LAN_BROADCAST_MAGIC)
                continue;

            broadcast->hostName[31] = 0;

            for (int pn = 0; pn < WIN64_LAN_BROADCAST_PLAYERS; pn++)
                broadcast->playerNames[pn][31] = '\0';

            char senderIP[64];
            inet_ntop(AF_INET, &senderAddr.sin_addr, senderIP, sizeof(senderIP));

            DWORD now = GetTickCount();

            pthread_mutex_lock(&s_discoveryLock);

            bool found = false;
            for (size_t i = 0; i < s_discoveredSessions.size(); i++)
            {
                if (strcmp(s_discoveredSessions[i].hostIP, senderIP) == 0 &&
                    s_discoveredSessions[i].hostPort == (int)broadcast->gamePort)
                {
                    s_discoveredSessions[i].netVersion = broadcast->netVersion;
                    for (int c = 0; c < 32; c++)
                        s_discoveredSessions[i].hostName[c] = (wchar_t)broadcast->hostName[c];
                    s_discoveredSessions[i].playerCount = broadcast->playerCount;
                    s_discoveredSessions[i].maxPlayers = broadcast->maxPlayers;
                    s_discoveredSessions[i].gameHostSettings = broadcast->gameHostSettings;
                    s_discoveredSessions[i].texturePackParentId = broadcast->texturePackParentId;
                    s_discoveredSessions[i].subTexturePackId = broadcast->subTexturePackId;
                    s_discoveredSessions[i].isJoinable = (broadcast->isJoinable != 0);
                    s_discoveredSessions[i].isDedicatedServer = (broadcast->isDedicatedServer != 0);
                    s_discoveredSessions[i].lastSeenTick = now;
                    memcpy(s_discoveredSessions[i].playerNames, broadcast->playerNames, sizeof(broadcast->playerNames));
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                if (s_discoveredSessions.size() >= MAX_DISCOVERED_SESSIONS)
                {
                    pthread_mutex_unlock(&s_discoveryLock);
                    continue;
                }

                Win64LANSession session;
                memset(&session, 0, sizeof(session));
                strncpy(session.hostIP, senderIP, sizeof(session.hostIP) - 1);
                session.hostPort = (int)broadcast->gamePort;
                session.netVersion = broadcast->netVersion;
                for (int c = 0; c < 32; c++)
                    session.hostName[c] = (wchar_t)broadcast->hostName[c];
                session.playerCount = broadcast->playerCount;
                session.maxPlayers = broadcast->maxPlayers;
                session.gameHostSettings = broadcast->gameHostSettings;
                session.texturePackParentId = broadcast->texturePackParentId;
                session.subTexturePackId = broadcast->subTexturePackId;
                session.isJoinable = (broadcast->isJoinable != 0);
                session.isDedicatedServer = (broadcast->isDedicatedServer != 0);
                session.lastSeenTick = now;
                memcpy(session.playerNames, broadcast->playerNames, sizeof(broadcast->playerNames));
                s_discoveredSessions.push_back(session);

                app.DebugPrintf("POSIX LAN: Discovered game \"%ls\" at %s:%d\n",
                    session.hostName, session.hostIP, session.hostPort);
            }

            for (size_t i = s_discoveredSessions.size(); i > 0; i--)
            {
                if (now - s_discoveredSessions[i - 1].lastSeenTick > 5000)
                {
                    app.DebugPrintf("POSIX LAN: Session \"%ls\" at %s timed out\n",
                        s_discoveredSessions[i - 1].hostName, s_discoveredSessions[i - 1].hostIP);
                    s_discoveredSessions.erase(s_discoveredSessions.begin() + (i - 1));
                }
            }

            pthread_mutex_unlock(&s_discoveryLock);
        }
    }

    return NULL;
}

#endif // __linux__
