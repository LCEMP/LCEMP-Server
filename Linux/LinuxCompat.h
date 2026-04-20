#pragma once


#ifdef __linux__

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cwchar>
#else
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#endif

#define __declspec(spec) __LINUX_DECLSPEC_##spec
#define __LINUX_DECLSPEC_thread __thread
#ifdef __cplusplus
#include <ctime>
#include <climits>
#include <cfloat>
#include <cmath>
#include <math.h>
using std::floor; using std::ceil; using std::cos; using std::sin;
using std::atan; using std::atan2; using std::sqrt; using std::fabs;

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#else
#include <time.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#endif

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

inline void _linuxFixPath(char* p) { for (; *p; ++p) if (*p == '\\') *p = '/'; }
inline void _linuxFixPathW(wchar_t* p) { for (; *p; ++p) if (*p == L'\\') *p = L'/'; }

static inline void _wfmtToNarrow(const wchar_t* wfmt, char* nfmt, size_t nfmtSize) {
    size_t ni = 0;
    for (size_t i = 0; wfmt[i] && ni < nfmtSize - 3; i++) {
        if (wfmt[i] == L'%') {
            nfmt[ni++] = '%'; i++;
            if (!wfmt[i]) break;
            if (wfmt[i] == L'%') { nfmt[ni++] = '%'; continue; }
            while (wfmt[i] == L'-' || wfmt[i] == L'+' || wfmt[i] == L' ' || wfmt[i] == L'#' || wfmt[i] == L'0')
                nfmt[ni++] = (char)wfmt[i++];
            if (wfmt[i] == L'*') { nfmt[ni++] = '*'; i++; }
            else while (wfmt[i] >= L'0' && wfmt[i] <= L'9')
                nfmt[ni++] = (char)wfmt[i++];
            if (wfmt[i] == L'.') {
                nfmt[ni++] = '.'; i++;
                if (wfmt[i] == L'*') { nfmt[ni++] = '*'; i++; }
                else while (wfmt[i] >= L'0' && wfmt[i] <= L'9')
                    nfmt[ni++] = (char)wfmt[i++];
            }
            if (wfmt[i] == L'l') { nfmt[ni++] = 'l'; i++; if (wfmt[i] == L'l') { nfmt[ni++] = 'l'; i++; } }
            else if (wfmt[i] == L'h') { nfmt[ni++] = 'h'; i++; if (wfmt[i] == L'h') { nfmt[ni++] = 'h'; i++; } }
            if (wfmt[i] == L's') { nfmt[ni++] = 'l'; nfmt[ni++] = 's'; }
            else if (wfmt[i] == L'S') { nfmt[ni++] = 's'; }
            else { nfmt[ni++] = (char)wfmt[i]; }
        } else {
            nfmt[ni++] = (char)wfmt[i];
        }
    }
    nfmt[ni] = '\0';
}

#ifdef linux
#undef linux
#endif
#ifdef windows
#undef windows
#endif

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef int16_t  SHORT;
typedef uint16_t USHORT;
typedef char     CHAR;
typedef float    FLOAT;

#ifdef __cplusplus
typedef bool     boolean;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef long long LONG64;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef int      BOOL;
typedef void     VOID;
typedef void*    LPVOID;
typedef void*    PVOID;
typedef const void* LPCVOID;
typedef char*    LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef unsigned char* PBYTE;
typedef DWORD*   PDWORD;
typedef BOOL*    PBOOL;
typedef size_t   SIZE_T;
typedef uintptr_t ULONG_PTR;
typedef unsigned int UINT;
typedef int      INT;
typedef int      HRESULT;
typedef int      errno_t;
typedef void*    HANDLE;
typedef void*    HWND;
typedef LONG*    PLONG;
typedef DWORD*   LPDWORD;
#ifndef __int64
#define __int64 long long
#endif
typedef unsigned long long __uint64;
#ifndef __int32
#define __int32 int
#endif
#ifndef __int16
#define __int16 short
#endif
#ifndef __int8
#define __int8 char
#endif

typedef wchar_t WCHAR;
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    LONGLONG QuadPart;
} LARGE_INTEGER;

#ifndef ZeroMemory
#define ZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#endif
#define CONST const
#define CALLBACK
#define MAKE_HRESULT(sev,fac,code) ((HRESULT)(((unsigned long)(sev)<<31)|((unsigned long)(fac)<<16)|((unsigned long)(code))))
#define __cdecl

#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#ifdef __cplusplus
#define NULL nullptr
#else
#define NULL ((void*)0)
#endif
#endif

#define VK_BACK      0x08
#define VK_TAB       0x09
#define VK_RETURN    0x0D
#define VK_ESCAPE    0x1B
#define VK_SPACE     0x20
#define VK_PRIOR     0x21
#define VK_NEXT      0x22
#define VK_LEFT      0x25
#define VK_UP        0x26
#define VK_RIGHT     0x27
#define VK_DOWN      0x28
#define VK_F3        0x72
#define VK_F4        0x73
#define VK_F5        0x74
#define VK_LSHIFT    0xA0
#define VK_LCONTROL  0xA2
#ifndef WHEEL_DELTA
#define WHEEL_DELTA  120
#endif

#define VK_PAD_A                0x5800
#define VK_PAD_B                0x5801
#define VK_PAD_X                0x5802
#define VK_PAD_Y                0x5803
#define VK_PAD_RSHOULDER        0x5804
#define VK_PAD_LSHOULDER        0x5805
#define VK_PAD_LTRIGGER         0x5806
#define VK_PAD_RTRIGGER         0x5807
#define VK_PAD_DPAD_UP          0x5810
#define VK_PAD_DPAD_DOWN        0x5811
#define VK_PAD_DPAD_LEFT        0x5812
#define VK_PAD_DPAD_RIGHT       0x5813
#define VK_PAD_START            0x5814
#define VK_PAD_BACK             0x5815
#define VK_PAD_LTHUMB_PRESS     0x5816
#define VK_PAD_LTHUMB_UP        0x5820
#define VK_PAD_LTHUMB_DOWN      0x5821
#define VK_PAD_LTHUMB_RIGHT     0x5822
#define VK_PAD_LTHUMB_LEFT      0x5823
#define VK_PAD_LTHUMB_UPLEFT    0x5824
#define VK_PAD_LTHUMB_UPRIGHT   0x5825
#define VK_PAD_LTHUMB_DOWNRIGHT 0x5826
#define VK_PAD_LTHUMB_DOWNLEFT  0x5827
#define VK_PAD_RTHUMB_UP        0x5830
#define VK_PAD_RTHUMB_DOWN      0x5831
#define VK_PAD_RTHUMB_RIGHT     0x5832
#define VK_PAD_RTHUMB_LEFT      0x5833
#define VK_PAD_RTHUMB_UPLEFT    0x5834
#define VK_PAD_RTHUMB_UPRIGHT   0x5835
#define VK_PAD_RTHUMB_DOWNRIGHT 0x5836
#define VK_PAD_RTHUMB_DOWNLEFT  0x5837

inline errno_t _wfopen_s(FILE** pFile, const wchar_t* filename, const wchar_t* mode) {
    if (!pFile) return EINVAL;
    char narrowName[1024];
    char narrowMode[64];
    wcstombs(narrowName, filename, sizeof(narrowName));
    _linuxFixPath(narrowName);
    wcstombs(narrowMode, mode, sizeof(narrowMode));
    char* comma = strchr(narrowMode, ',');
    if (comma) *comma = '\0';
    *pFile = fopen(narrowName, narrowMode);
    return (*pFile) ? 0 : errno;
}

inline int _wtoi(const wchar_t* str) {
    return (int)wcstol(str, NULL, 10);
}

inline long long _wtoi64(const wchar_t* str) {
    return wcstoll(str, NULL, 10);
}

inline errno_t fopen_s(FILE** pFile, const char* filename, const char* mode) {
    if (!pFile) return EINVAL;
    char fixed[1024];
    strncpy(fixed, filename, sizeof(fixed) - 1); fixed[sizeof(fixed) - 1] = '\0';
    _linuxFixPath(fixed);
    *pFile = fopen(fixed, mode);
    return (*pFile) ? 0 : errno;
}

#define _stricmp strcasecmp
#define _wcsicmp wcscasecmp

inline unsigned long long _wcstoui64(const wchar_t* str, wchar_t** endptr, int base) {
    return wcstoull(str, endptr, base);
}

inline DWORD GetCurrentDirectoryA(DWORD nBufferLength, char* lpBuffer) {
    if (getcwd(lpBuffer, nBufferLength))
        return (DWORD)strlen(lpBuffer);
    return 0;
}

#define S_OK 0
#define S_FALSE 1
#define E_FAIL (-1)
#define HRESULT_SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define SUCCEEDED(hr)  (((HRESULT)(hr)) >= 0)
#define FAILED(hr)     (((HRESULT)(hr)) < 0)

#define ERROR_IO_PENDING  997L
#define ERROR_CANCELLED   1223L

#define CDECL

#define CP_ACP 0

inline int WideCharToMultiByte(unsigned int codePage, unsigned long dwFlags,
    const wchar_t* lpWideCharStr, int cchWideChar,
    char* lpMultiByteStr, int cbMultiByte,
    const char* lpDefaultChar, int* lpUsedDefaultChar) {
    (void)codePage; (void)dwFlags; (void)lpDefaultChar; (void)lpUsedDefaultChar;
    if (cchWideChar == -1) cchWideChar = (int)wcslen(lpWideCharStr) + 1;
    if (cbMultiByte == 0) return cchWideChar; // query size
    return (int)wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte);
}

inline int __vsnprintf_s_impl(char* buffer, size_t sizeOfBuffer, size_t count, const char* format, va_list argptr) {
    if (count == _TRUNCATE) count = sizeOfBuffer - 1;
    return vsnprintf(buffer, count + 1, format, argptr);
}
#define _vsnprintf_s(buf, count, fmt, ap) __vsnprintf_s_impl(buf, sizeof(buf), count, fmt, ap)

#define swscanf_s swscanf

inline errno_t _itoa_s(int value, char* buffer, size_t sizeOfBuffer, int radix) {
    if (radix == 10) { snprintf(buffer, sizeOfBuffer, "%d", value); return 0; }
    if (radix == 16) { snprintf(buffer, sizeOfBuffer, "%x", value); return 0; }
    char* p = buffer;
    char* end = buffer + sizeOfBuffer - 1;
    int isNeg = (value < 0 && radix == 10);
    unsigned int uval = isNeg ? (unsigned int)-value : (unsigned int)value;
    char tmp[65]; int i = 0;
    do { int d = uval % radix; tmp[i++] = (d < 10) ? ('0'+d) : ('a'+d-10); uval /= radix; } while(uval && i < 64);
    if (isNeg) tmp[i++] = '-';
    for (int j = i-1; j >= 0 && p < end; j--) *p++ = tmp[j];
    *p = '\0';
    return 0;
}

inline errno_t _i64toa_s(long long value, char* buffer, size_t sizeOfBuffer, int radix) {
    if (radix == 10) { snprintf(buffer, sizeOfBuffer, "%lld", value); return 0; }
    if (radix == 16) { snprintf(buffer, sizeOfBuffer, "%llx", value); return 0; }
    snprintf(buffer, sizeOfBuffer, "%lld", value);
    return 0;
}

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#endif

#ifndef GENERIC_READ
#define GENERIC_READ 0x80000000
#endif
#ifndef GENERIC_WRITE
#define GENERIC_WRITE 0x40000000
#endif

#ifndef OPEN_EXISTING
#define OPEN_EXISTING 3
#endif
#ifndef OPEN_ALWAYS
#define OPEN_ALWAYS 4
#endif

#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0x80
#endif
#ifndef FILE_FLAG_RANDOM_ACCESS
#define FILE_FLAG_RANDOM_ACCESS 0x10000000
#endif
#ifndef FILE_FLAG_SEQUENTIAL_SCAN
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#endif

#ifndef FILE_BEGIN
#define FILE_BEGIN 0
#endif
#ifndef FILE_END
#define FILE_END 2
#endif

#ifndef PAGE_READWRITE
#define PAGE_READWRITE 0x04
#endif
#ifndef MEM_COMMIT
#define MEM_COMMIT 0x1000
#endif
#ifndef MEM_RESERVE
#define MEM_RESERVE 0x2000
#endif
#ifndef MEM_DECOMMIT
#define MEM_DECOMMIT 0x4000
#endif
#ifndef MEM_RELEASE
#define MEM_RELEASE 0x8000
#endif
#ifndef MEM_LARGE_PAGES
#define MEM_LARGE_PAGES 0x20000000
#endif

#ifndef MAXULONG_PTR
#define MAXULONG_PTR ((ULONG_PTR)~0)
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0L
#endif

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;

typedef pthread_mutex_t CRITICAL_SECTION;

inline void InitializeCriticalSection(CRITICAL_SECTION* cs)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(cs, &attr);
    pthread_mutexattr_destroy(&attr);
}

inline void InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION* cs, DWORD spinCount)
{
    (void)spinCount;
    InitializeCriticalSection(cs);
}

inline void EnterCriticalSection(CRITICAL_SECTION* cs)
{
    pthread_mutex_lock(cs);
}

inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* cs)
{
    return pthread_mutex_trylock(cs) == 0 ? TRUE : FALSE;
}

inline void LeaveCriticalSection(CRITICAL_SECTION* cs)
{
    pthread_mutex_unlock(cs);
}

inline void DeleteCriticalSection(CRITICAL_SECTION* cs)
{
    pthread_mutex_destroy(cs);
}

enum LinuxHandleType { LINUX_HANDLE_EVENT, LINUX_HANDLE_FILE };

struct LinuxEvent {
    LinuxHandleType handleType;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            signaled;
    bool            autoReset;
};

struct LinuxFileHandle {
    LinuxHandleType handleType;
    int             fd;
};

inline HANDLE CreateEventW(void* /*sec*/, BOOL manualReset, BOOL initialState, const wchar_t* /*name*/)
{
    LinuxEvent* ev = new LinuxEvent();
    ev->handleType = LINUX_HANDLE_EVENT;
    pthread_mutex_init(&ev->mutex, NULL);
    pthread_cond_init(&ev->cond, NULL);
    ev->signaled = (initialState != 0);
    ev->autoReset = (manualReset == 0);
    return (HANDLE)ev;
}

inline HANDLE CreateEvent(void* sec, BOOL manualReset, BOOL initialState, const char* /*name*/)
{
    return CreateEventW(sec, manualReset, initialState, NULL);
}

inline BOOL SetEvent(HANDLE h)
{
    LinuxEvent* ev = (LinuxEvent*)h;
    pthread_mutex_lock(&ev->mutex);
    ev->signaled = true;
    pthread_cond_broadcast(&ev->cond);
    pthread_mutex_unlock(&ev->mutex);
    return TRUE;
}

inline BOOL ResetEvent(HANDLE h)
{
    LinuxEvent* ev = (LinuxEvent*)h;
    pthread_mutex_lock(&ev->mutex);
    ev->signaled = false;
    pthread_mutex_unlock(&ev->mutex);
    return TRUE;
}

#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT  0x102
#define INFINITE      0xFFFFFFFF
#define STILL_ACTIVE  259
#define CREATE_SUSPENDED 0x00000004

inline DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds)
{
    LinuxEvent* ev = (LinuxEvent*)h;
    pthread_mutex_lock(&ev->mutex);
    if (milliseconds == INFINITE)
    {
        while (!ev->signaled)
            pthread_cond_wait(&ev->cond, &ev->mutex);
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec  += milliseconds / 1000;
        ts.tv_nsec += (milliseconds % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) { ts.tv_sec++; ts.tv_nsec -= 1000000000; }

        while (!ev->signaled)
        {
            int rc = pthread_cond_timedwait(&ev->cond, &ev->mutex, &ts);
            if (rc == ETIMEDOUT) break;
        }
    }
    bool was = ev->signaled;
    if (ev->autoReset) ev->signaled = false;
    pthread_mutex_unlock(&ev->mutex);
    return was ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}

inline BOOL CloseHandle(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
    LinuxHandleType type = *(LinuxHandleType*)h;
    if (type == LINUX_HANDLE_FILE)
    {
        LinuxFileHandle* fh = (LinuxFileHandle*)h;
        int rc = close(fh->fd);
        delete fh;
        return rc == 0 ? TRUE : FALSE;
    }
    LinuxEvent* ev = (LinuxEvent*)h;
    pthread_mutex_destroy(&ev->mutex);
    pthread_cond_destroy(&ev->cond);
    delete ev;
    return TRUE;
}

#ifndef GENERIC_READ
#define GENERIC_READ  0x80000000
#endif
#ifndef GENERIC_WRITE
#define GENERIC_WRITE 0x40000000
#endif
#ifndef CREATE_ALWAYS
#define CREATE_ALWAYS 2
#endif
#ifndef OPEN_EXISTING
#define OPEN_EXISTING 3
#endif
#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ 1
#endif
#ifndef FILE_SHARE_WRITE
#define FILE_SHARE_WRITE 2
#endif

inline HANDLE CreateFile(const char* path, DWORD access, DWORD /*share*/, void* /*sec*/, DWORD creation, DWORD /*flags*/, HANDLE /*templ*/)
{
    if (!path) return INVALID_HANDLE_VALUE;
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);

    int oflags = 0;
    if ((access & GENERIC_READ) && (access & GENERIC_WRITE)) oflags = O_RDWR;
    else if (access & GENERIC_WRITE) oflags = O_WRONLY;
    else oflags = O_RDONLY;

    if (creation == CREATE_ALWAYS) oflags |= O_CREAT | O_TRUNC;
    else if (creation == OPEN_EXISTING) { /* no extra flags */ }

    int fd = open(fixed, oflags, 0644);
    if (fd < 0) return INVALID_HANDLE_VALUE;

    LinuxFileHandle* fh = new LinuxFileHandle();
    fh->handleType = LINUX_HANDLE_FILE;
    fh->fd = fd;
    return (HANDLE)fh;
}
#define CreateFileA CreateFile

#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#endif

inline DWORD GetFileSize(HANDLE h, DWORD* high)
{
    if (!h || h == INVALID_HANDLE_VALUE) return INVALID_FILE_SIZE;
    LinuxFileHandle* fh = (LinuxFileHandle*)h;
    struct stat st;
    if (fstat(fh->fd, &st) != 0) return INVALID_FILE_SIZE;
    if (high) *high = (DWORD)((st.st_size >> 32) & 0xFFFFFFFF);
    return (DWORD)(st.st_size & 0xFFFFFFFF);
}

inline BOOL ReadFile(HANDLE h, void* buf, DWORD toRead, DWORD* readOut, void* /*ov*/)
{
    if (!h || h == INVALID_HANDLE_VALUE) { if (readOut) *readOut = 0; return FALSE; }
    LinuxFileHandle* fh = (LinuxFileHandle*)h;
    ssize_t n = read(fh->fd, buf, toRead);
    if (n < 0) { if (readOut) *readOut = 0; return FALSE; }
    if (readOut) *readOut = (DWORD)n;
    return TRUE;
}

inline BOOL WriteFile(HANDLE h, const void* buf, DWORD toWrite, DWORD* writtenOut, void* /*ov*/)
{
    if (!h || h == INVALID_HANDLE_VALUE) { if (writtenOut) *writtenOut = 0; return FALSE; }
    LinuxFileHandle* fh = (LinuxFileHandle*)h;
    ssize_t n = write(fh->fd, buf, toWrite);
    if (n < 0) { if (writtenOut) *writtenOut = 0; return FALSE; }
    if (writtenOut) *writtenOut = (DWORD)n;
    return TRUE;
}

inline DWORD GetLastError()
{
    return (DWORD)errno;
}

inline LPVOID VirtualAlloc(LPVOID /*addr*/, size_t size, DWORD /*allocType*/, DWORD /*protect*/)
{
    return malloc(size);
}

inline BOOL VirtualFree(LPVOID addr, size_t /*size*/, DWORD /*freeType*/)
{
    free(addr);
    return TRUE;
}

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

inline void GetSystemTime(LPSYSTEMTIME st)
{
    if (!st) return;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm t;
    gmtime_r(&ts.tv_sec, &t);
    st->wYear = (WORD)(t.tm_year + 1900);
    st->wMonth = (WORD)(t.tm_mon + 1);
    st->wDayOfWeek = (WORD)t.tm_wday;
    st->wDay = (WORD)t.tm_mday;
    st->wHour = (WORD)t.tm_hour;
    st->wMinute = (WORD)t.tm_min;
    st->wSecond = (WORD)t.tm_sec;
    st->wMilliseconds = (WORD)(ts.tv_nsec / 1000000);
}

inline BOOL SystemTimeToFileTime(const SYSTEMTIME* st, FILETIME* ft)
{
    if (!st || !ft) return FALSE;
    struct tm t = {};
    t.tm_year = st->wYear - 1900;
    t.tm_mon  = st->wMonth - 1;
    t.tm_mday = st->wDay;
    t.tm_hour = st->wHour;
    t.tm_min  = st->wMinute;
    t.tm_sec  = st->wSecond;
    time_t epoch = timegm(&t);
    uint64_t ticks = ((uint64_t)epoch + 11644473600ULL) * 10000000ULL + (uint64_t)st->wMilliseconds * 10000ULL;
    ft->dwLowDateTime  = (DWORD)(ticks & 0xFFFFFFFF);
    ft->dwHighDateTime = (DWORD)(ticks >> 32);
    return TRUE;
}

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    size_t dwTotalPhys;
    size_t dwAvailPhys;
    size_t dwTotalPageFile;
    size_t dwAvailPageFile;
    size_t dwTotalVirtual;
    size_t dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

inline void GlobalMemoryStatus(LPMEMORYSTATUS lpBuffer)
{
    if (!lpBuffer) return;
    memset(lpBuffer, 0, sizeof(MEMORYSTATUS));
    lpBuffer->dwLength = sizeof(MEMORYSTATUS);
}

inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* v)
{
    if (!v) return FALSE;
    v->QuadPart = 1000000000LL;
    return TRUE;
}

inline BOOL QueryPerformanceCounter(LARGE_INTEGER* v)
{
    if (!v) return FALSE;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    v->QuadPart = (LONGLONG)ts.tv_sec * 1000000000LL + (LONGLONG)ts.tv_nsec;
    return TRUE;
}

inline LONGLONG InterlockedCompareExchangeRelease64(volatile LONGLONG* destination, LONGLONG exchange, LONGLONG comparand)
{
    return __sync_val_compare_and_swap(destination, comparand, exchange);
}

inline LONG InterlockedCompareExchange(volatile LONG* destination, LONG exchange, LONG comparand)
{
    return __sync_val_compare_and_swap(destination, comparand, exchange);
}

inline LONG InterlockedCompareExchangeRelease(volatile LONG* destination, LONG exchange, LONG comparand)
{
    return __sync_val_compare_and_swap(destination, comparand, exchange);
}

inline LONG InterlockedExchange(volatile LONG* target, LONG value)
{
    return __sync_lock_test_and_set(target, value);
}

typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID);

struct LinuxThreadData {
    LPTHREAD_START_ROUTINE func;
    LPVOID param;
};

inline void* LinuxThreadWrapper(void* arg)
{
    LinuxThreadData* td = (LinuxThreadData*)arg;
    LPTHREAD_START_ROUTINE fn = td->func;
    LPVOID p = td->param;
    delete td;
    DWORD result = fn(p);
    return (void*)(uintptr_t)result;
}

inline HANDLE CreateThread(void* /*sec*/, size_t /*stack*/, LPTHREAD_START_ROUTINE func, LPVOID param, DWORD /*flags*/, DWORD* /*tid*/)
{
    pthread_t* pt = new pthread_t;
    LinuxThreadData* td = new LinuxThreadData{func, param};
    if (pthread_create(pt, NULL, LinuxThreadWrapper, td) != 0) {
        delete td;
        delete pt;
        return NULL;
    }
    return (HANDLE)pt;
}

inline std::vector<pthread_key_t>& LinuxTlsKeys()
{
    static std::vector<pthread_key_t> keys;
    return keys;
}

inline CRITICAL_SECTION& LinuxTlsLock()
{
    static CRITICAL_SECTION lock;
    static bool initialized = false;
    if (!initialized)
    {
        InitializeCriticalSection(&lock);
        initialized = true;
    }
    return lock;
}

inline DWORD TlsAlloc()
{
    pthread_key_t key;
    if (pthread_key_create(&key, NULL) != 0)
        return 0xFFFFFFFF;

    EnterCriticalSection(&LinuxTlsLock());
    std::vector<pthread_key_t>& keys = LinuxTlsKeys();
    keys.push_back(key);
    DWORD idx = (DWORD)(keys.size() - 1);
    LeaveCriticalSection(&LinuxTlsLock());
    return idx;
}

inline BOOL TlsSetValue(DWORD idx, LPVOID value)
{
    EnterCriticalSection(&LinuxTlsLock());
    std::vector<pthread_key_t>& keys = LinuxTlsKeys();
    if (idx >= keys.size())
    {
        LeaveCriticalSection(&LinuxTlsLock());
        return FALSE;
    }
    pthread_key_t key = keys[idx];
    LeaveCriticalSection(&LinuxTlsLock());
    return pthread_setspecific(key, value) == 0 ? TRUE : FALSE;
}

inline LPVOID TlsGetValue(DWORD idx)
{
    EnterCriticalSection(&LinuxTlsLock());
    std::vector<pthread_key_t>& keys = LinuxTlsKeys();
    if (idx >= keys.size())
    {
        LeaveCriticalSection(&LinuxTlsLock());
        return NULL;
    }
    pthread_key_t key = keys[idx];
    LeaveCriticalSection(&LinuxTlsLock());
    return (LPVOID)pthread_getspecific(key);
}


inline void Sleep(DWORD ms) { usleep(ms * 1000); }

inline DWORD GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (DWORD)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

inline int sprintf_s(char* buf, size_t sz, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, sz, fmt, ap);
    va_end(ap);
    return r;
}

inline int sprintf_s(char* buf, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, 256, fmt, ap);
    va_end(ap);
    return r;
}

static inline void _wfmtFixWide(const wchar_t* src, wchar_t* dst, size_t dstSize) {
    size_t di = 0;
    for (size_t i = 0; src[i] && di < dstSize - 3; i++) {
        if (src[i] == L'%') {
            dst[di++] = L'%'; i++;
            if (!src[i]) break;
            if (src[i] == L'%') { dst[di++] = L'%'; continue; }
            while (src[i] == L'-' || src[i] == L'+' || src[i] == L' ' || src[i] == L'#' || src[i] == L'0')
                dst[di++] = src[i++];
            if (src[i] == L'*') { dst[di++] = L'*'; i++; }
            else while (src[i] >= L'0' && src[i] <= L'9')
                dst[di++] = src[i++];
            if (src[i] == L'.') {
                dst[di++] = L'.'; i++;
                if (src[i] == L'*') { dst[di++] = L'*'; i++; }
                else while (src[i] >= L'0' && src[i] <= L'9')
                    dst[di++] = src[i++];
            }
            if (src[i] == L'l') { dst[di++] = L'l'; i++; if (src[i] == L'l') { dst[di++] = L'l'; i++; } }
            else if (src[i] == L'h') { dst[di++] = L'h'; i++; if (src[i] == L'h') { dst[di++] = L'h'; i++; } }
            if (src[i] == L's') { dst[di++] = L'l'; dst[di++] = L's'; }
            else if (src[i] == L'S') { dst[di++] = L's'; }
            else { dst[di++] = src[i]; }
        } else {
            dst[di++] = src[i];
        }
    }
    dst[di] = L'\0';
}

inline int swprintf_s(wchar_t* buf, size_t sz, const wchar_t* fmt, ...)
{
    wchar_t fixedFmt[2048];
    _wfmtFixWide(fmt, fixedFmt, 2048);
    va_list ap;
    va_start(ap, fmt);
    int r = vswprintf(buf, sz, fixedFmt, ap);
    va_end(ap);
    return r;
}

inline int wcscpy_s(wchar_t* dst, size_t sz, const wchar_t* src)
{
    wcsncpy(dst, src, sz);
    if (sz > 0) dst[sz - 1] = L'\0';
    return 0;
}

inline int wcsncpy_s(wchar_t* dst, size_t dstsz, const wchar_t* src, size_t count)
{
    size_t to_copy = (count < dstsz - 1) ? count : dstsz - 1;
    wcsncpy(dst, src, to_copy);
    dst[to_copy] = L'\0';
    return 0;
}

#ifdef __cplusplus
template<size_t N>
inline int wcsncpy_s(wchar_t (&dst)[N], const wchar_t* src, size_t count)
{
    return wcsncpy_s(dst, N, src, count);
}
#endif // __cplusplus

#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif

inline int strncpy_s(char* dst, size_t dstsz, const char* src, size_t count)
{
    if (count == _TRUNCATE) count = dstsz - 1;
    size_t to_copy = (count < dstsz - 1) ? count : dstsz - 1;
    strncpy(dst, src, to_copy);
    dst[to_copy] = '\0';
    return 0;
}

inline int strcpy_s(char* dst, size_t dstsz, const char* src)
{
    if (!dst || dstsz == 0) return -1;
    strncpy(dst, src, dstsz - 1);
    dst[dstsz - 1] = '\0';
    return 0;
}

template<size_t N>
inline int strcpy_s(char (&dst)[N], const char* src)
{
    return strcpy_s(dst, N, src);
}

inline errno_t localtime_s(struct tm* result, const time_t* timer)
{
    return localtime_r(timer, result) ? 0 : errno;
}

inline wchar_t* _itow(int value, wchar_t* str, int radix)
{
    if (radix < 2 || radix > 36) { str[0] = L'\0'; return str; }
    wchar_t* p = str;
    bool neg = (value < 0 && radix == 10);
    unsigned int uval = neg ? (unsigned int)(-(long long)value) : (unsigned int)value;
    wchar_t buf[34];
    int i = 0;
    do {
        int d = uval % radix;
        buf[i++] = (wchar_t)(d < 10 ? L'0' + d : L'a' + d - 10);
        uval /= radix;
    } while (uval);
    if (neg) *p++ = L'-';
    while (i > 0) *p++ = buf[--i];
    *p = L'\0';
    return str;
}

inline BOOL SetConsoleOutputCP(unsigned int) { return TRUE; }
inline BOOL SetConsoleCP(unsigned int) { return TRUE; }
inline BOOL SetConsoleTitleW(const wchar_t*) { return TRUE; }
inline BOOL FreeConsole() { return TRUE; }
inline void ExitProcess(int code) { _exit(code); }

inline BOOL CreateDirectoryW(const wchar_t* path, void*)
{
    char mbpath[MAX_PATH];
    wcstombs(mbpath, path, MAX_PATH);
    _linuxFixPath(mbpath);
    return mkdir(mbpath, 0755) == 0 || errno == EEXIST;
}

inline DWORD GetModuleFileNameW(void*, wchar_t* buf, DWORD sz)
{
    char path[MAX_PATH];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len <= 0) return 0;
    path[len] = '\0';
    mbstowcs(buf, path, sz);
    return (DWORD)wcslen(buf);
}

inline BOOL SetCurrentDirectoryW(const wchar_t* path)
{
    char mbpath[MAX_PATH];
    wcstombs(mbpath, path, MAX_PATH);
    _linuxFixPath(mbpath);
    return chdir(mbpath) == 0;
}

#define CP_UTF8 65001
#define WINAPI

#define __debugbreak() __builtin_trap()

#define WM_CLOSE 0x0010
inline BOOL PostMessage(HWND, unsigned int, unsigned long, long) { return TRUE; }

inline DWORD GetCurrentThreadId()
{
    return (DWORD)(uintptr_t)pthread_self();
}

inline HANDLE GetCurrentThread()
{
    return (HANDLE)(uintptr_t)pthread_self();
}

inline DWORD ResumeThread(HANDLE /*h*/)
{
    return 0;
}

#ifndef THREAD_PRIORITY_LOWEST
#define THREAD_PRIORITY_LOWEST        (-2)
#endif
#ifndef THREAD_PRIORITY_BELOW_NORMAL
#define THREAD_PRIORITY_BELOW_NORMAL  (-1)
#endif
#ifndef THREAD_PRIORITY_NORMAL
#define THREAD_PRIORITY_NORMAL        0
#endif
#ifndef THREAD_PRIORITY_ABOVE_NORMAL
#define THREAD_PRIORITY_ABOVE_NORMAL  1
#endif
#ifndef THREAD_PRIORITY_HIGHEST
#define THREAD_PRIORITY_HIGHEST       2
#endif

inline BOOL SetThreadPriority(HANDLE /*h*/, int /*priority*/)
{
    return TRUE;
}

inline BOOL GetExitCodeThread(HANDLE /*h*/, DWORD* code)
{
    if (code) *code = 0;
    return TRUE;
}

inline DWORD WaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD timeout)
{
    if (!handles || count == 0) return WAIT_TIMEOUT;
    if (waitAll)
    {
        for (DWORD i = 0; i < count; ++i)
        {
            DWORD r = WaitForSingleObject(handles[i], timeout);
            if (r != WAIT_OBJECT_0) return r;
        }
        return WAIT_OBJECT_0;
    }
    return WaitForSingleObject(handles[0], timeout);
}

typedef struct tagRECT { LONG left, top, right, bottom; } RECT;
typedef struct tagPOINT { LONG x, y; } POINT;

typedef struct { unsigned short usUsagePage; unsigned short usUsage; DWORD dwFlags; HWND hwndTarget; } RAWINPUTDEVICE;
#define RIDEV_NOLEGACY 0
#define HID_USAGE_PAGE_GENERIC 1
#define HID_USAGE_GENERIC_MOUSE 2
#define HID_USAGE_GENERIC_KEYBOARD 6
inline BOOL RegisterRawInputDevices(const RAWINPUTDEVICE*, unsigned int, unsigned int) { return TRUE; }
inline BOOL GetClientRect(HWND, RECT*) { return FALSE; }
inline BOOL ClientToScreen(HWND, POINT*) { return FALSE; }
inline BOOL SetCursorPos(int, int) { return FALSE; }
inline int ShowCursor(BOOL) { return 0; }
inline BOOL ClipCursor(const RECT*) { return FALSE; }

#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#endif
#ifndef FILE_ATTRIBUTE_READONLY
#define FILE_ATTRIBUTE_READONLY 0x01
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif


inline DWORD GetFileAttributes(const char* path)
{
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);
    struct stat st;
    if (stat(fixed, &st) != 0) return INVALID_FILE_ATTRIBUTES;
    DWORD attrs = FILE_ATTRIBUTE_NORMAL;
    if (S_ISDIR(st.st_mode)) attrs |= FILE_ATTRIBUTE_DIRECTORY;
    return attrs;
}
#define GetFileAttributesA GetFileAttributes

inline BOOL DeleteFile(const char* path)
{
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);
    return (remove(fixed) == 0) ? TRUE : FALSE;
}

inline BOOL CreateDirectory(const char* path, void* /*sec*/)
{
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);
    return (mkdir(fixed, 0755) == 0 || errno == EEXIST) ? TRUE : FALSE;
}
#define CreateDirectoryA CreateDirectory

inline BOOL MoveFile(const char* src, const char* dst)
{
    char fsrc[MAX_PATH], fdst[MAX_PATH];
    strncpy(fsrc, src, MAX_PATH - 1); fsrc[MAX_PATH - 1] = '\0'; _linuxFixPath(fsrc);
    strncpy(fdst, dst, MAX_PATH - 1); fdst[MAX_PATH - 1] = '\0'; _linuxFixPath(fdst);
    return (rename(fsrc, fdst) == 0) ? TRUE : FALSE;
}


struct WIN32_FIND_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    char     cFileName[MAX_PATH];
    char     cAlternateFileName[14];
};

struct LinuxFindHandle {
    DIR*        dir;
    std::string basePath;
};

inline HANDLE FindFirstFile(const char* pattern, WIN32_FIND_DATA* wfd)
{
    if (!pattern || !wfd) return INVALID_HANDLE_VALUE;

    char normalized[MAX_PATH];
    strncpy(normalized, pattern, MAX_PATH - 1); normalized[MAX_PATH - 1] = '\0';
    _linuxFixPath(normalized);

    std::string pat(normalized);
    size_t star = pat.rfind('*');
    if (star != std::string::npos) pat = pat.substr(0, star);
    while (!pat.empty() && (pat.back() == '/' || pat.back() == '\\'))
        pat.pop_back();
    if (pat.empty()) pat = ".";

    DIR* d = opendir(pat.c_str());
    if (!d) return INVALID_HANDLE_VALUE;

    struct dirent* entry = readdir(d);
    if (!entry) { closedir(d); return INVALID_HANDLE_VALUE; }

    memset(wfd, 0, sizeof(WIN32_FIND_DATA));
    strncpy(wfd->cFileName, entry->d_name, MAX_PATH - 1);

    std::string fullPath = pat + "/" + entry->d_name;
    struct stat st;
    if (stat(fullPath.c_str(), &st) == 0)
    {
        if (S_ISDIR(st.st_mode)) wfd->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        else wfd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        wfd->nFileSizeLow = (DWORD)(st.st_size & 0xFFFFFFFF);
        wfd->nFileSizeHigh = (DWORD)((st.st_size >> 32) & 0xFFFFFFFF);
    }

    LinuxFindHandle* fh = new LinuxFindHandle();
    fh->dir = d;
    fh->basePath = pat;
    return (HANDLE)fh;
}

inline BOOL FindNextFile(HANDLE h, WIN32_FIND_DATA* wfd)
{
    if (!h || h == INVALID_HANDLE_VALUE || !wfd) return FALSE;
    LinuxFindHandle* fh = (LinuxFindHandle*)h;
    struct dirent* entry = readdir(fh->dir);
    if (!entry) return FALSE;

    memset(wfd, 0, sizeof(WIN32_FIND_DATA));
    strncpy(wfd->cFileName, entry->d_name, MAX_PATH - 1);

    std::string fullPath = fh->basePath + "/" + entry->d_name;
    struct stat st;
    if (stat(fullPath.c_str(), &st) == 0)
    {
        if (S_ISDIR(st.st_mode)) wfd->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        else wfd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        wfd->nFileSizeLow = (DWORD)(st.st_size & 0xFFFFFFFF);
        wfd->nFileSizeHigh = (DWORD)((st.st_size >> 32) & 0xFFFFFFFF);
    }
    return TRUE;
}

inline BOOL FindClose(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
    LinuxFindHandle* fh = (LinuxFindHandle*)h;
    closedir(fh->dir);
    delete fh;
    return TRUE;
}
typedef WIN32_FIND_DATA _WIN32_FIND_DATAA;
typedef WIN32_FIND_DATA WIN32_FIND_DATAA;
#define FindFirstFileA FindFirstFile
#define FindNextFileA  FindNextFile

enum FINDEX_INFO_LEVELS { FindExInfoStandard = 0, FindExInfoBasic = 1 };
enum FINDEX_SEARCH_OPS  { FindExSearchNameMatch = 0, FindExSearchLimitToDirectories = 1 };

inline HANDLE FindFirstFileExA(const char* pattern, FINDEX_INFO_LEVELS /*lvl*/, void* findData,
                                FINDEX_SEARCH_OPS /*searchOp*/, void* /*filter*/, DWORD /*flags*/)
{
    return FindFirstFile(pattern, (WIN32_FIND_DATA*)findData);
}

#define DeleteFileA DeleteFile

inline BOOL RemoveDirectoryA(const char* path)
{
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);
    return (rmdir(fixed) == 0) ? TRUE : FALSE;
}

inline BOOL CopyFileA(const char* src, const char* dst, BOOL failIfExists)
{
    char srcFixed[MAX_PATH], dstFixed[MAX_PATH];
    strncpy(srcFixed, src, MAX_PATH - 1); srcFixed[MAX_PATH - 1] = '\0';
    strncpy(dstFixed, dst, MAX_PATH - 1); dstFixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(srcFixed);
    _linuxFixPath(dstFixed);

    if (failIfExists) {
        struct stat st;
        if (stat(dstFixed, &st) == 0) return FALSE;
    }
    int in = open(srcFixed, O_RDONLY);
    if (in < 0) return FALSE;
    int out = open(dstFixed, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { close(in); return FALSE; }
    char buf[8192];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        ssize_t w = write(out, buf, n);
        if (w != n) { close(in); close(out); return FALSE; }
    }
    close(in);
    close(out);
    return TRUE;
}

inline int MultiByteToWideChar(unsigned int /*codePage*/, unsigned long /*flags*/,
                                const char* mbStr, int mbLen,
                                wchar_t* wcStr, int wcLen)
{
    if (!mbStr) return 0;
    size_t srcLen = (mbLen == -1) ? strlen(mbStr) + 1 : (size_t)mbLen;
    if (wcLen == 0 || !wcStr) return (int)srcLen; // estimate
    size_t toCopy = (srcLen < (size_t)wcLen) ? srcLen : (size_t)wcLen;
    for (size_t i = 0; i < toCopy; i++) wcStr[i] = (wchar_t)(unsigned char)mbStr[i];
    return (int)toCopy;
}

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA;

#define GetFileExInfoStandard 0

inline BOOL GetFileAttributesEx(const char* path, int /*level*/, void* info)
{
    if (!path || !info) return FALSE;
    WIN32_FILE_ATTRIBUTE_DATA* data = (WIN32_FILE_ATTRIBUTE_DATA*)info;
    char fixed[MAX_PATH];
    strncpy(fixed, path, MAX_PATH - 1); fixed[MAX_PATH - 1] = '\0';
    _linuxFixPath(fixed);
    struct stat st;
    if (stat(fixed, &st) != 0) return FALSE;

    memset(data, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    if (S_ISDIR(st.st_mode)) data->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

    data->nFileSizeLow  = (DWORD)(st.st_size & 0xFFFFFFFF);
    data->nFileSizeHigh = (DWORD)((st.st_size >> 32) & 0xFFFFFFFF);

    uint64_t ticks = ((uint64_t)st.st_mtime + 11644473600ULL) * 10000000ULL;
    data->ftLastWriteTime.dwLowDateTime  = (DWORD)(ticks & 0xFFFFFFFF);
    data->ftLastWriteTime.dwHighDateTime = (DWORD)(ticks >> 32);

    return TRUE;
}
#define GetFileAttributesExA GetFileAttributesEx

inline void OutputDebugStringA(const char*) {}
inline void OutputDebugStringW(const wchar_t*) {}
#define OutputDebugString OutputDebugStringA

using std::wstring;
using std::string;

#endif // __cplusplus

#endif // __linux__
