#include "stdafx.h"

#if defined(__linux__) || defined(_WIN32)

#include "ServerThreadPool.h"
#include "../../Minecraft.World/AABB.h"
#include "../../Minecraft.World/Vec3.h"
#include "../../Minecraft.World/IntCache.h"
#include "../../Minecraft.World/compression.h"
#include "../../Minecraft.World/Tile.h"
#include "../../Minecraft.World/Level.h"

#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#include <time.h>
#endif
#include <cstring>

#ifdef __linux__
std::vector<pthread_t> ServerThreadPool::s_workers;
pthread_mutex_t ServerThreadPool::s_queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ServerThreadPool::s_queueCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ServerThreadPool::s_doneCond = PTHREAD_COND_INITIALIZER;
#else
std::vector<HANDLE> ServerThreadPool::s_workers;
CRITICAL_SECTION ServerThreadPool::s_queueLock;
CONDITION_VARIABLE ServerThreadPool::s_queueCond;
CONDITION_VARIABLE ServerThreadPool::s_doneCond;
#endif

std::queue<ServerThreadPool::Task> ServerThreadPool::s_taskQueue;
std::atomic<int> ServerThreadPool::s_pendingTasks(0);
volatile bool ServerThreadPool::s_running = false;
int ServerThreadPool::s_threadCount = 0;
bool ServerThreadPool::s_initialized = false;

#ifdef __linux__
static bool JoinWorkerWithTimeout(pthread_t worker, int timeoutMs)
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

    int rc = pthread_timedjoin_np(worker, NULL, &ts);
    if (rc == 0)
        return true;

    pthread_detach(worker);
    return false;
#else
    int waitedMs = 0;
    while (waitedMs < timeoutMs)
    {
        if (pthread_tryjoin_np(worker, NULL) == 0)
            return true;

        usleep(10 * 1000);
        waitedMs += 10;
    }

    pthread_detach(worker);
    return false;
#endif
}
#endif

void ServerThreadPool::WorkerInit()
{
    AABB::CreateNewThreadStorage();
    Vec3::CreateNewThreadStorage();
    IntCache::CreateNewThreadStorage();
    Compression::CreateNewThreadStorage();
    Level::enableLightingCache();
    Tile::CreateNewThreadStorage();
}

#ifdef __linux__
void *ServerThreadPool::WorkerProc(void *param)
#else
DWORD WINAPI ServerThreadPool::WorkerProc(LPVOID param)
#endif
{
    WorkerInit();

    while (s_running)
    {
        Task task;
        bool gotTask = false;

#ifdef __linux__
        pthread_mutex_lock(&s_queueLock);
        while (s_running && s_taskQueue.empty())
            pthread_cond_wait(&s_queueCond, &s_queueLock);

        if (!s_running && s_taskQueue.empty())
        {
            pthread_mutex_unlock(&s_queueLock);
            break;
        }

        if (!s_taskQueue.empty())
        {
            task = s_taskQueue.front();
            s_taskQueue.pop();
            gotTask = true;
        }
        pthread_mutex_unlock(&s_queueLock);
#else
        EnterCriticalSection(&s_queueLock);
        while (s_running && s_taskQueue.empty())
            SleepConditionVariableCS(&s_queueCond, &s_queueLock, INFINITE);

        if (!s_running && s_taskQueue.empty())
        {
            LeaveCriticalSection(&s_queueLock);
            break;
        }

        if (!s_taskQueue.empty())
        {
            task = s_taskQueue.front();
            s_taskQueue.pop();
            gotTask = true;
        }
        LeaveCriticalSection(&s_queueLock);
#endif

        if (gotTask && task.func != NULL)
        {
            try
            {
                task.func(task.param);
            }
            catch (...)
            {
            }

            if (s_pendingTasks.fetch_sub(1) == 1)
            {
#ifdef __linux__
                pthread_mutex_lock(&s_queueLock);
                pthread_cond_broadcast(&s_doneCond);
                pthread_mutex_unlock(&s_queueLock);
#else
                EnterCriticalSection(&s_queueLock);
                WakeAllConditionVariable(&s_doneCond);
                LeaveCriticalSection(&s_queueLock);
#endif
            }
        }
    }

    AABB::ReleaseThreadStorage();
    Vec3::ReleaseThreadStorage();
    IntCache::ReleaseThreadStorage();
    Tile::ReleaseThreadStorage();
    Level::destroyLightingCache();

#ifdef __linux__
    return NULL;
#else
    return 0;
#endif
}

bool ServerThreadPool::Initialize(int threadCount)
{
    if (s_initialized) return true;

    if (threadCount <= 0)
    {
#ifdef __linux__
        long cores = sysconf(_SC_NPROCESSORS_ONLN);
#else
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        long cores = (long)sysInfo.dwNumberOfProcessors;
#endif
        if (cores < 2) cores = 2;
        threadCount = (int)(cores - 1);
        if (threadCount > 16) threadCount = 16;
    }

#ifdef __linux__
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&s_queueLock, &attr);
    pthread_mutexattr_destroy(&attr);
    pthread_cond_init(&s_queueCond, NULL);
    pthread_cond_init(&s_doneCond, NULL);
#else
    InitializeCriticalSection(&s_queueLock);
    InitializeConditionVariable(&s_queueCond);
    InitializeConditionVariable(&s_doneCond);
#endif

    s_running = true;
    s_threadCount = threadCount;

    s_workers.resize(threadCount);
    for (int i = 0; i < threadCount; i++)
    {
#ifdef __linux__
        pthread_create(&s_workers[i], NULL, WorkerProc, NULL);
#else
        s_workers[i] = CreateThread(NULL, 0, WorkerProc, NULL, 0, NULL);
#endif
    }

    s_initialized = true;
    return true;
}

void ServerThreadPool::Shutdown()
{
    if (!s_initialized) return;

    s_running = false;
    s_initialized = false;

#ifdef __linux__
    pthread_mutex_lock(&s_queueLock);
    while (!s_taskQueue.empty())
        s_taskQueue.pop();
    pthread_cond_broadcast(&s_queueCond);
    pthread_mutex_unlock(&s_queueLock);

    bool allWorkersJoined = true;
    for (int i = 0; i < (int)s_workers.size(); i++)
    {
        if (!JoinWorkerWithTimeout(s_workers[i], 5000))
            allWorkersJoined = false;
    }
#else
    EnterCriticalSection(&s_queueLock);
    while (!s_taskQueue.empty())
        s_taskQueue.pop();
    WakeAllConditionVariable(&s_queueCond);
    LeaveCriticalSection(&s_queueLock);

    for (int i = 0; i < (int)s_workers.size(); i++)
    {
        WaitForSingleObject(s_workers[i], 5000);
        CloseHandle(s_workers[i]);
    }
#endif

    s_workers.clear();
    s_pendingTasks.store(0);

#ifdef __linux__
    if (allWorkersJoined)
    {
        pthread_mutex_destroy(&s_queueLock);
        pthread_cond_destroy(&s_queueCond);
        pthread_cond_destroy(&s_doneCond);
    }
#else
    DeleteCriticalSection(&s_queueLock);
#endif

    s_threadCount = 0;
}

int ServerThreadPool::GetThreadCount()
{
    return s_threadCount;
}

void ServerThreadPool::Submit(TaskFunc func, void *param)
{
    if (!s_initialized || func == NULL) return;

    Task task;
    task.func = func;
    task.param = param;

    s_pendingTasks.fetch_add(1);

#ifdef __linux__
    pthread_mutex_lock(&s_queueLock);
    s_taskQueue.push(task);
    pthread_cond_signal(&s_queueCond);
    pthread_mutex_unlock(&s_queueLock);
#else
    EnterCriticalSection(&s_queueLock);
    s_taskQueue.push(task);
    WakeConditionVariable(&s_queueCond);
    LeaveCriticalSection(&s_queueLock);
#endif
}

void ServerThreadPool::SubmitBatch(Task *tasks, int count)
{
    if (!s_initialized || tasks == NULL || count <= 0) return;

    s_pendingTasks.fetch_add(count);

#ifdef __linux__
    pthread_mutex_lock(&s_queueLock);
    for (int i = 0; i < count; i++)
        s_taskQueue.push(tasks[i]);
    pthread_cond_broadcast(&s_queueCond);
    pthread_mutex_unlock(&s_queueLock);
#else
    EnterCriticalSection(&s_queueLock);
    for (int i = 0; i < count; i++)
        s_taskQueue.push(tasks[i]);
    WakeAllConditionVariable(&s_queueCond);
    LeaveCriticalSection(&s_queueLock);
#endif
}

void ServerThreadPool::WaitAll()
{
    if (!s_initialized) return;

#ifdef __linux__
    pthread_mutex_lock(&s_queueLock);
    while (s_pendingTasks.load() > 0)
        pthread_cond_wait(&s_doneCond, &s_queueLock);
    pthread_mutex_unlock(&s_queueLock);
#else
    EnterCriticalSection(&s_queueLock);
    while (s_pendingTasks.load() > 0)
        SleepConditionVariableCS(&s_doneCond, &s_queueLock, INFINITE);
    LeaveCriticalSection(&s_queueLock);
#endif
}

static void ParallelForTaskFunc(void *param)
{
    ParallelForData *data = (ParallelForData *)param;
    if (data != NULL && data->func != NULL)
        data->func(data->index, data->param);
}

void ServerThreadPool::ParallelFor(int start, int end, void (*func)(int index, void *param), void *param)
{
    int count = end - start;
    if (count <= 0 || func == NULL) return;

    if (count == 1 || !s_initialized)
    {
        for (int i = start; i < end; i++)
            func(i, param);
        return;
    }

    std::vector<ParallelForData> taskData(count);
    std::vector<Task> tasks(count);

    for (int i = 0; i < count; i++)
    {
        taskData[i].func = func;
        taskData[i].param = param;
        taskData[i].index = start + i;
        tasks[i].func = ParallelForTaskFunc;
        tasks[i].param = &taskData[i];
    }

    SubmitBatch(&tasks[0], count);
    WaitAll();
}

#endif
