#include "stdafx.h"
#include "ServerLogger.h"

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <string>

using std::string;
using std::wstring;

#ifdef __linux__
#include <pthread.h>
static pthread_mutex_t g_logMutex = PTHREAD_MUTEX_INITIALIZER;
#else
static CRITICAL_SECTION g_logCS;
#endif

static FILE* g_logFile = NULL;
static bool g_initialized = false;
static wstring* g_inputBuffer = nullptr;

void ServerLog_Init()
{
    if (g_initialized) return;

#ifndef __linux__
    InitializeCriticalSection(&g_logCS);
#endif

#ifdef __linux__
    g_logFile = fopen("server.log", "a");
#else
    g_logFile = _wfopen(L"server.log", L"a");
#endif

    if (!g_logFile)
    {
        fprintf(stderr, "WARNING: Failed to log to server.log\n");
    }

    g_initialized = true;
}

void ServerLog_Close()
{
    if (!g_initialized) return;

    if (g_logFile)
    {
        fclose(g_logFile);
        g_logFile = NULL;
    }

#ifndef __linux__
    DeleteCriticalSection(&g_logCS);
#endif

    g_initialized = false;
}

#ifdef __linux__

static string processNarrowFormattingCodes(const char* input)
{
    string result;
    bool hasColor = false;
    for (size_t i = 0; input[i]; i++)
    {
        if ((unsigned char)input[i] == 0xC2 && (unsigned char)input[i + 1] == 0xA7 && input[i + 2])
        {
            char code = tolower((unsigned char)input[i + 2]);
            i += 2;
            hasColor = true;
            switch (code)
            {
                case '0': result += "\x1b[30m"; break;
                case '1': result += "\x1b[34m"; break;
                case '2': result += "\x1b[32m"; break;
                case '3': result += "\x1b[36m"; break;
                case '4': result += "\x1b[31m"; break;
                case '5': result += "\x1b[35m"; break;
                case '6': result += "\x1b[33m"; break;
                case '7': result += "\x1b[37m"; break;
                case '8': result += "\x1b[90m"; break;
                case '9': result += "\x1b[94m"; break;
                case 'a': result += "\x1b[92m"; break;
                case 'b': result += "\x1b[96m"; break;
                case 'c': result += "\x1b[91m"; break;
                case 'd': result += "\x1b[95m"; break;
                case 'e': result += "\x1b[93m"; break;
                case 'f': result += "\x1b[97m"; break;
                case 'l': result += "\x1b[1m"; break;
                case 'm': result += "\x1b[9m"; break;
                case 'n': result += "\x1b[4m"; break;
                case 'o': result += "\x1b[3m"; break;
                case 'r': result += "\x1b[0m"; break;
                default: break;
            }
        }
        else
        {
            result += input[i];
        }
    }
    if (hasColor) result += "\x1b[0m";
    return result;
}

static string stripNarrowFormattingCodes(const char* input)
{
    string result;
    for (size_t i = 0; input[i]; i++)
    {
        if ((unsigned char)input[i] == 0xC2 && (unsigned char)input[i + 1] == 0xA7 && input[i + 2])
            i += 2;
        else
            result += input[i];
    }
    return result;
}

static void logImpl(const char* level, const wchar_t* fmt, va_list ap)
{
    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);

    char narrowFmt[2048];
    _wfmtToNarrow(fmt, narrowFmt, sizeof(narrowFmt));

    char narrowMsg[4096];
    vsnprintf(narrowMsg, sizeof(narrowMsg), narrowFmt, ap);

    string consoleMsg = processNarrowFormattingCodes(narrowMsg);

    pthread_mutex_lock(&g_logMutex);

    if (g_inputBuffer && !g_inputBuffer->empty())
        printf("\r\033[K");

    printf("%04d-%02d-%02d %02d:%02d:%02d [%s] %s",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
           t.tm_hour, t.tm_min, t.tm_sec, level, consoleMsg.c_str());
    fflush(stdout);

    if (g_logFile)
    {
        string fileMsg = stripNarrowFormattingCodes(narrowMsg);
        fprintf(g_logFile, "%04d-%02d-%02d %02d:%02d:%02d [%s] %s",
                t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                t.tm_hour, t.tm_min, t.tm_sec, level, fileMsg.c_str());
        fflush(g_logFile);
    }

    if (g_inputBuffer && !g_inputBuffer->empty())
    {
        printf("%ls", g_inputBuffer->c_str());
        fflush(stdout);
    }

    pthread_mutex_unlock(&g_logMutex);
}

#else

static wstring stripWideFormattingCodes(const wchar_t* input)
{
    wstring result;
    for (size_t i = 0; input[i]; i++)
    {
        if (input[i] == L'\u00A7' && input[i + 1])
            i++;
        else
            result += input[i];
    }
    return result;
}

struct ColorSpan {
    size_t start;
    size_t end;
    WORD attr;
};

static void printColored(const wchar_t* text)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD defaultAttr = csbi.wAttributes;
    WORD currentAttr = defaultAttr;

    const wchar_t* p = text;
    while (*p)
    {
        if (*p == L'\u00A7' && *(p + 1))
        {
            wchar_t code = towlower(*(p + 1));
            p += 2;
            switch (code)
            {
                case L'0': currentAttr = 0; break;
                case L'1': currentAttr = FOREGROUND_BLUE; break;
                case L'2': currentAttr = FOREGROUND_GREEN; break;
                case L'3': currentAttr = FOREGROUND_GREEN | FOREGROUND_BLUE; break;
                case L'4': currentAttr = FOREGROUND_RED; break;
                case L'5': currentAttr = FOREGROUND_RED | FOREGROUND_BLUE; break;
                case L'6': currentAttr = FOREGROUND_RED | FOREGROUND_GREEN; break;
                case L'7': currentAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
                case L'8': currentAttr = FOREGROUND_INTENSITY; break;
                case L'9': currentAttr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
                case L'a': currentAttr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
                case L'b': currentAttr = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
                case L'c': currentAttr = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
                case L'd': currentAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
                case L'e': currentAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
                case L'f': currentAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
                case L'l': currentAttr |= FOREGROUND_INTENSITY; break;
                case L'r': currentAttr = defaultAttr; break;
                default: break;
            }
            SetConsoleTextAttribute(hConsole, currentAttr);
        }
        else
        {
            wchar_t ch[2] = { *p, 0 };
            wprintf(L"%ls", ch);
            p++;
        }
    }
    SetConsoleTextAttribute(hConsole, defaultAttr);
}

static void logImpl(const char* level, const wchar_t* fmt, va_list ap)
{
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);

    wchar_t wbuf[4096];
    vswprintf_s(wbuf, 4096, fmt, ap);

    wstring stripped = stripWideFormattingCodes(wbuf);

    EnterCriticalSection(&g_logCS);

    if (g_inputBuffer && !g_inputBuffer->empty())
    {
        wstring spaces(g_inputBuffer->size(), L' ');
        wprintf(L"\r%ls\r", spaces.c_str());
    }

    wprintf(L"%04d-%02d-%02d %02d:%02d:%02d [%S] ",
            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec, level);
    printColored(wbuf);

    if (g_logFile)
    {
        char narrowMsg[4096];
        size_t converted;
        wcstombs_s(&converted, narrowMsg, sizeof(narrowMsg), stripped.c_str(), _TRUNCATE);

        fprintf(g_logFile, "%04d-%02d-%02d %02d:%02d:%02d [%s] %s",
                t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                t.tm_hour, t.tm_min, t.tm_sec, level, narrowMsg);
        fflush(g_logFile);
    }

    if (g_inputBuffer && !g_inputBuffer->empty())
        wprintf(L"%ls", g_inputBuffer->c_str());

    LeaveCriticalSection(&g_logCS);
}

#endif // __linux__

void ServerLog(const wchar_t* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logImpl("INFO", fmt, ap);
    va_end(ap);
}

void ServerWarn(const wchar_t* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logImpl("WARNING", fmt, ap);
    va_end(ap);
}

void ServerLog_RegisterInput(wstring* buffer)
{
    g_inputBuffer = buffer;
}

void ServerLog_UnregisterInput()
{
    g_inputBuffer = nullptr;
}

#ifdef __linux__
void ServerLog_LockOutput() { pthread_mutex_lock(&g_logMutex); }
void ServerLog_UnlockOutput() { pthread_mutex_unlock(&g_logMutex); }
#else
void ServerLog_LockOutput() { EnterCriticalSection(&g_logCS); }
void ServerLog_UnlockOutput() { LeaveCriticalSection(&g_logCS); }
#endif
