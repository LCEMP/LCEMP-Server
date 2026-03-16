# LCEMP Dedicated Server

A dedicated server for Legacy Console Edition. This allows you to host a standalone server for LCE.
Supports a maximum of 32 players and Voice Chat. (max 8 players on voice chatfor now)

## Building

You need the main LCEMP repository to build the dedicated server, as it depends on the shared Minecraft.World and Minecraft.Client sources.

### Linux

Requires CMake 3.10+ and a C++ compiler with C++11 support (GCC or Clang).

```bash
mkdir build && cd build
cmake ../Minecraft.Server -DCMAKE_BUILD_TYPE=Release
cmake --build . --target MinecraftDedicatedServer
```

The binary will be at `build/MinecraftDedicatedServer`.

### Windows

Open `Minecraft.Server.vcxproj` in Visual Studio (2012 or later with the v110 toolset, or 2022 with v143) and build.

## Running

On Linux, run the binary directly:

```bash
./MinecraftDedicatedServer
```

On Windows, run `MinecraftDedicatedServer.exe` from a command prompt or by double-clicking it.

On first launch the server generates a default `server.properties` file, which you can edit to configure the server.

## server.properties

| Property | Type | Default | Description |
|---|---|---|---|
| `server-port` | integer | `25565` | Port the server listens on. |
| `server-ip` | string | *(empty)* | IP address to bind to. Leave empty to bind on all interfaces. |
| `level-name` | string | `world` | Name of the world. |
| `level-seed` | integer | *(empty)* | World seed. Leave empty for a random seed. |
| `level-size` | string | `large` | World size for newly created worlds. Options: `classic`, `small`, `medium`, `large`. |
| `gamemode` | integer | `0` | Default game mode. `0` = Survival, `1` = Creative. |
| `difficulty` | integer | `2` | Difficulty level. `0` = Peaceful, `1` = Easy, `2` = Normal, `3` = Hard. |
| `max-players` | integer | `8` | Maximum number of players (1-8). |
| `pvp` | boolean | `true` | Whether players can damage each other. |
| `trust-players` | boolean | `true` | Whether to trust player positions sent by the client. |
| `fire-spreads` | boolean | `true` | Whether fire spreads to nearby blocks. |
| `tnt-explodes` | boolean | `true` | Whether TNT explosions are enabled. |
| `structures` | boolean | `true` | Whether structures generate in new worlds. |
| `spawn-animals` | boolean | `true` | Whether animals spawn naturally. |
| `spawn-npcs` | boolean | `true` | Whether villagers spawn naturally. |
| `online-mode` | boolean | `false` | Reserved for future use. |
| `show-gamertags` | boolean | `true` | Whether player name tags are visible in-game. |
| `motd` | string | `A Minecraft LCE Server` | Server name shown in the LAN browser. |
| `white-list` | boolean | `false` | Whether to use a whitelist. Only players listed in `whitelist.txt` can join. |
| `voice-chat` | boolean | `false` | Whether voice chat is enabled. Only works with 8 or fewer max players. |
| `advertise-lan` | boolean | `true` | Whether the server broadcasts itself on the local network so clients can find it automatically. |

### World sizes

| Size | Overworld | Nether scale |
|---|---|---|
| `classic` | 864x864 blocks | 3x |
| `small` | 1024x1024 blocks | 3x |
| `medium` | 3072x3072 blocks | 6x |
| `large` | 5120x5120 blocks | 8x |

The `level-size` property only takes effect when the server creates a new world. Existing saves keep their original size.

## Commands

Commands can be typed directly into the server console. Use `/help` to see a paginated list or `/help <command>` for details on a specific command (these commands are the same as vanilla java minecraft).

### Player Management

| Command | Usage | Description |
|---|---|---|
| `op` | `/op <player>` | Grants operator status to a player. |
| `deop` | `/deop <player>` | Removes operator status from a player. |
| `kick` | `/kick <player> [reason]` | Kicks a player from the server. |
| `ban` | `/ban <player>` | Bans a player by name. |
| `ban-ip` | `/ban-ip <address or player>` | Bans an IP address, or resolves a player name to their IP and bans it. |
| `pardon` | `/pardon <player>` | Unbans a player. |
| `pardon-ip` | `/pardon-ip <address>` | Unbans an IP address. |
| `banlist` | `/banlist [ips\|players]` | Shows the ban list. Defaults to players. |
| `whitelist` | `/whitelist <on\|off\|list\|add\|remove\|reload> [player]` | Manages the whitelist. |
| `list` | `/list` | Shows online players. |

### Gameplay

| Command | Usage | Description |
|---|---|---|
| `gamemode` | `/gamemode <mode> [player]` | Changes a player's game mode. Accepts `0`/`s`/`survival`, `1`/`c`/`creative`, `2`/`a`/`adventure`. |
| `defaultgamemode` | `/defaultgamemode <mode>` | Sets the default game mode for new players joining. |
| `give` | `/give <player> <item id> [amount] [data]` | Gives an item to a player. |
| `enchant` | `/enchant <player> <enchantment id> [level]` | Enchants the item a player is holding. |
| `tp` | `/tp [player] <target player>` or `/tp [player] <x> <y> <z>` | Teleports a player to another player or to coordinates. |
| `kill` | `/kill <player>` | Kills a player. |
| `xp` | `/xp <amount> [player]` or `/xp <amount>L [player]` | Gives experience. Append `L` to give levels instead of points. |

### World

| Command | Usage | Description |
|---|---|---|
| `time` | `/time <set\|add> <value>` | Sets or adds to the world time. Accepts `day`, `night`, or a number. |
| `toggledownfall` | `/toggledownfall` | Toggles rain/snow on or off. |
| `seed` | `/seed` | Displays the world seed. |
| `save-all` | `/save-all` | Forces an immediate save of the world. |
| `save-off` | `/save-off` | Disables automatic saving. |
| `save-on` | `/save-on` | Re-enables automatic saving. |

### Other

| Command | Usage | Description |
|---|---|---|
| `say` | `/say <message>` | Broadcasts a message to all players as `[Server]`. |
| `me` | `/me <action>` | Broadcasts an action message as `* Server <action>`. |
| `help` | `/help [page or command]` | Shows the command list or usage for a specific command. |
| `stop` | `/stop` | Saves and shuts down the server. |
| `debug` | `/debug <start\|stop>` | Starts or stops a debug profiling session. |
