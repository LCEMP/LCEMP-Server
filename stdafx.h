#pragma once

#ifndef _DEDICATED_SERVER
#define _DEDICATED_SERVER
#endif

#ifdef __linux__
typedef unsigned __int128 __uint128;
#endif

#include "../Minecraft.Client/stdafx.h"

#include "../Minecraft.World/TilePos.h"

#define FIFTY_ONE_MB (1000000*51)