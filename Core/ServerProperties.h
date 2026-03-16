#pragma once

#include <string>
#include <map>

using namespace std;

class ServerProperties
{
public:
    ServerProperties();

    bool load(const wstring& path);
    void save(const wstring& path);

    wstring getString(const wstring& key, const wstring& defaultVal);
    int getInt(const wstring& key, int defaultVal);
    __int64 getInt64(const wstring& key, __int64 defaultVal);
    bool getBool(const wstring& key, bool defaultVal);

    int serverPort;
    wstring levelName;
    __int64 levelSeed;
    int gamemode;
    int difficulty;
    int maxPlayers;
    bool pvp;
    bool trustPlayers;
    bool fireSpreads;
    bool tntExplodes;
    bool structures;
    bool spawnAnimals;
    bool spawnNpcs;
    bool onlineMode;
    bool showGamertags;
    wstring motd;
    bool whiteList;
    bool voiceChat;
    wstring levelSize;
    bool advertiseLan;
    wstring serverIp;

private:
    map<wstring, wstring> m_properties;
    void loadDefaults();
};
