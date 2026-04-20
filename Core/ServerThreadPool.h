#pragma once

#if defined(__linux__) || defined(_WIN32)

#ifdef __linux__
#include <pthread.h>
#else
#include <windows.h>
#endif

#include <vector>
#include <queue>
#include <functional>
#include <atomic>

#define SERVER_THREAD_POOL_DEFAULT_SIZE 0

class ServerThreadPool
{
public:
    typedef void (*TaskFunc)(void *param);

    struct Task
    {
        TaskFunc func;
        void *param;
    };

    static bool Initialize(int threadCount = SERVER_THREAD_POOL_DEFAULT_SIZE);
    static void Shutdown();
    static int GetThreadCount();
    static bool IsInitialized() { return s_initialized; }

    static void Submit(TaskFunc func, void *param);
    static void SubmitBatch(Task *tasks, int count);
    static void WaitAll();

    static void ParallelFor(int start, int end, void (*func)(int index, void *param), void *param);

private:
#ifdef __linux__
    static void *WorkerProc(void *param);
#else
    static DWORD WINAPI WorkerProc(LPVOID param);
#endif
    static void WorkerInit();

#ifdef __linux__
    static std::vector<pthread_t> s_workers;
    static pthread_mutex_t s_queueLock;
    static pthread_cond_t s_queueCond;
    static pthread_cond_t s_doneCond;
#else
    static std::vector<HANDLE> s_workers;
    static CRITICAL_SECTION s_queueLock;
    static CONDITION_VARIABLE s_queueCond;
    static CONDITION_VARIABLE s_doneCond;
#endif
    static std::queue<Task> s_taskQueue;
    static std::atomic<int> s_pendingTasks;
    static volatile bool s_running;
    static int s_threadCount;
    static bool s_initialized;
};

struct ParallelForData
{
    void (*func)(int index, void *param);
    void *param;
    int index;
};

#endif
