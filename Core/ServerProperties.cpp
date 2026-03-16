#include "stdafx.h"
#include "ServerProperties.h"
#include <fstream>
#include <sstream>
#include <ctime>

ServerProperties::ServerProperties()
{
    loadDefaults();
}

void ServerProperties::loadDefaults()
{
    serverPort = 25565;
    levelName = L"world";
    levelSeed = 0;
    gamemode = 0;
    difficulty = 2;
    maxPlayers = 8;
    pvp = true;
    trustPlayers = true;
    fireSpreads = true;
    tntExplodes = true;
    structures = true;
    spawnAnimals = true;
    spawnNpcs = true;
    onlineMode = false;
    showGamertags = true;
    motd = L"A Minecraft LCE Server";
    whiteList = false;
    voiceChat = false;
    levelSize = L"large";
    advertiseLan = true;
    serverIp = L"";
}

static wstring trimWs(const wstring& s)
{
    size_t start = s.find_first_not_of(L" \t\r\n");
    if (start == wstring::npos) return L"";
    size_t end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool ServerProperties::load(const wstring& path)
{
    FILE *f = NULL;
    _wfopen_s(&f, path.c_str(), L"r, ccs=UTF-8");
    if (!f)
        return false;

    wchar_t line[1024];
    while (fgetws(line, 1024, f))
    {
        wstring ws(line);
        size_t nl = ws.find_last_not_of(L"\r\n");
        if (nl != wstring::npos)
            ws = ws.substr(0, nl + 1);
        else
            ws.clear();

        if (ws.empty()) continue;
        if (ws[0] == L'#') continue;

        size_t eq = ws.find(L'=');
        if (eq == wstring::npos) continue;

        wstring key = trimWs(ws.substr(0, eq));
        wstring val = trimWs(ws.substr(eq + 1));
        m_properties[key] = val;
    }
    fclose(f);

    serverPort = getInt(L"server-port", 25565);
    if (serverPort <= 0 || serverPort > 65535) serverPort = 25565;
    levelName = getString(L"level-name", L"world");
    levelSeed = getInt64(L"level-seed", 0);
    gamemode = getInt(L"gamemode", 0);
    difficulty = getInt(L"difficulty", 2);
    maxPlayers = getInt(L"max-players", 8);
    if (maxPlayers < 1) maxPlayers = 1;
    pvp = getBool(L"pvp", true);
    trustPlayers = getBool(L"trust-players", true);
    fireSpreads = getBool(L"fire-spreads", true);
    tntExplodes = getBool(L"tnt-explodes", true);
    structures = getBool(L"structures", true);
    spawnAnimals = getBool(L"spawn-animals", true);
    spawnNpcs = getBool(L"spawn-npcs", true);
    onlineMode = getBool(L"online-mode", false);
    showGamertags = getBool(L"show-gamertags", true);
    motd = getString(L"motd", L"A Minecraft LCE Server");
    whiteList = getBool(L"white-list", false);
    voiceChat = getBool(L"voice-chat", false);
    levelSize = getString(L"level-size", L"large");
    advertiseLan = getBool(L"advertise-lan", true);
    serverIp = getString(L"server-ip", L"");

    return true;
}

void ServerProperties::save(const wstring& path)
{
    FILE *f = NULL;
    _wfopen_s(&f, path.c_str(), L"w, ccs=UTF-8");
    if (!f) return;

    fwprintf(f, L"#Minecraft server properties\n");
    {
        time_t now = time(NULL);
        struct tm t;
        localtime_s(&t, &now);
        static const wchar_t* dayNames[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
        static const wchar_t* monNames[] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
        fwprintf(f, L"#%ls %ls %02d %02d:%02d:%02d %04d\n",
                 dayNames[t.tm_wday], monNames[t.tm_mon], t.tm_mday,
                 t.tm_hour, t.tm_min, t.tm_sec, t.tm_year + 1900);
    }
    fwprintf(f, L"server-port=%d\n", serverPort);
    fwprintf(f, L"level-name=%ls\n", levelName.c_str());
    if (levelSeed != 0)
        fwprintf(f, L"level-seed=%lld\n", levelSeed);
    else
        fwprintf(f, L"level-seed=\n");
    fwprintf(f, L"gamemode=%d\n", gamemode);
    fwprintf(f, L"difficulty=%d\n", difficulty);
    fwprintf(f, L"max-players=%d\n", maxPlayers);
    fwprintf(f, L"pvp=%ls\n", pvp ? L"true" : L"false");
    fwprintf(f, L"trust-players=%ls\n", trustPlayers ? L"true" : L"false");
    fwprintf(f, L"fire-spreads=%ls\n", fireSpreads ? L"true" : L"false");
    fwprintf(f, L"tnt-explodes=%ls\n", tntExplodes ? L"true" : L"false");
    fwprintf(f, L"structures=%ls\n", structures ? L"true" : L"false");
    fwprintf(f, L"spawn-animals=%ls\n", spawnAnimals ? L"true" : L"false");
    fwprintf(f, L"spawn-npcs=%ls\n", spawnNpcs ? L"true" : L"false");
    fwprintf(f, L"online-mode=%ls\n", onlineMode ? L"true" : L"false");
    fwprintf(f, L"show-gamertags=%ls\n", showGamertags ? L"true" : L"false");
    fwprintf(f, L"motd=%ls\n", motd.c_str());
    fwprintf(f, L"white-list=%ls\n", whiteList ? L"true" : L"false");
    fwprintf(f, L"voice-chat=%ls\n", voiceChat ? L"true" : L"false");
    fwprintf(f, L"level-size=%ls\n", levelSize.c_str());
    fwprintf(f, L"advertise-lan=%ls\n", advertiseLan ? L"true" : L"false");
    if (!serverIp.empty())
        fwprintf(f, L"server-ip=%ls\n", serverIp.c_str());
    else
        fwprintf(f, L"server-ip=\n");

    fclose(f);
}

wstring ServerProperties::getString(const wstring& key, const wstring& defaultVal)
{
    map<wstring, wstring>::iterator it = m_properties.find(key);
    if (it != m_properties.end())
        return it->second;
    return defaultVal;
}

int ServerProperties::getInt(const wstring& key, int defaultVal)
{
    map<wstring, wstring>::iterator it = m_properties.find(key);
    if (it != m_properties.end() && !it->second.empty())
        return _wtoi(it->second.c_str());
    return defaultVal;
}

__int64 ServerProperties::getInt64(const wstring& key, __int64 defaultVal)
{
    map<wstring, wstring>::iterator it = m_properties.find(key);
    if (it != m_properties.end() && !it->second.empty())
        return _wtoi64(it->second.c_str());
    return defaultVal;
}

bool ServerProperties::getBool(const wstring& key, bool defaultVal)
{
    map<wstring, wstring>::iterator it = m_properties.find(key);
    if (it != m_properties.end())
    {
        wstring val = it->second;
        if (val == L"true" || val == L"1") return true;
        if (val == L"false" || val == L"0") return false;
    }
    return defaultVal;
}
