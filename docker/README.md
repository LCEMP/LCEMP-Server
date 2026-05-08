# Docker

This directory builds and runs the Linux dedicated server in a container.

Build from the LCEMP repository root, not from `Minecraft.Server`, because the server CMake project depends on sibling `Minecraft.Client`, `Minecraft.World`, and `cmake` directories.

On Windows, use the build wrapper from `Minecraft.Server/docker`. It uses this order:

1. Copy an existing Linux binary from `build/MinecraftDedicatedServer`.
2. Download a GitHub release asset when `GITHUB_REPOSITORY` is configured.
3. Fall back to the full Docker build.

```powershell
.\build-image.ps1
```

On Linux/macOS:

```bash
./build-image.sh
```

You can also call Docker directly for a full build:

```bash
docker build -f Minecraft.Server/docker/Dockerfile -t lcemp-server .
```

The Dockerfile defaults to `BUILD_JOBS=2` to avoid overloading Docker Desktop/WSL during the large C++ compile. Override it on stronger Linux hosts:

```bash
docker build -f Minecraft.Server/docker/Dockerfile -t lcemp-server --build-arg BUILD_JOBS=8 .
```

To force the wrapper to do a full build:

```powershell
.\build-image.ps1 -ForceFullBuild
```

To download a prebuilt binary from a public GitHub release before falling back to the full build:

```powershell
.\build-image.ps1 `
  -GitHubRepo "owner/repo" `
  -GitHubReleaseTag "latest" `
  -GitHubAssetName "MinecraftDedicatedServer"
```

The shell wrapper uses equivalent environment variables:

```bash
GITHUB_REPOSITORY=owner/repo \
GITHUB_RELEASE_TAG=latest \
GITHUB_ASSET_NAME=MinecraftDedicatedServer \
./build-image.sh
```

To build a runtime image from an already-built Linux binary directly:

```bash
docker build -f Minecraft.Server/docker/Dockerfile.prebuilt -t lcemp-server .
```

Run with a persistent data directory:

```bash
docker run --rm -it \
  -p 25565:25565/tcp \
  -p 25565:25565/udp \
  -p 25566:25566/udp \
  -v lcemp-data:/data \
  -e MOTD="A Minecraft LCE Server" \
  -e LEVEL_NAME="world" \
  lcemp-server
```

Or use Compose from this directory:

```bash
docker compose up --build
```

To use Compose with an existing `build/MinecraftDedicatedServer` binary:

```bash
docker compose -f compose.yaml -f compose.prebuilt.yaml up --build
```

The entrypoint writes `/data/server.properties` from environment variables on every start, then launches `MinecraftDedicatedServer` from `/data` so generated worlds, lists, and server data persist in the mounted volume.

## Environment Variables

| Environment variable | server.properties key | Default |
|---|---|---|
| `SERVER_PORT` | `server-port` | `25565` |
| `SERVER_IP` | `server-ip` | empty |
| `LEVEL_NAME` | `level-name` | `world` |
| `LEVEL_SEED` | `level-seed` | empty |
| `LEVEL_SIZE` | `level-size` | `large` |
| `GAMEMODE` | `gamemode` | `0` |
| `DIFFICULTY` | `difficulty` | `2` |
| `MAX_PLAYERS` | `max-players` | `8` |
| `PVP` | `pvp` | `true` |
| `TRUST_PLAYERS` | `trust-players` | `true` |
| `FIRE_SPREADS` | `fire-spreads` | `true` |
| `TNT_EXPLODES` | `tnt-explodes` | `true` |
| `STRUCTURES` | `structures` | `true` |
| `SPAWN_ANIMALS` | `spawn-animals` | `true` |
| `SPAWN_NPCS` | `spawn-npcs` | `true` |
| `ONLINE_MODE` | `online-mode` | `false` |
| `SHOW_GAMERTAGS` | `show-gamertags` | `true` |
| `MOTD` | `motd` | `A Minecraft LCE Server` |
| `WHITE_LIST` | `white-list` | `false` |
| `VOICE_CHAT` | `voice-chat` | `false` |
| `ENABLE_CHAT` | `enable-chat` | `false` |
| `ADVERTISE_LAN` | `advertise-lan` | `true` |

`SERVER_PROPERTIES_FILE` can override the generated properties path. The default is `/data/server.properties`.

LAN discovery uses UDP broadcast. If clients cannot discover the server through Docker port publishing, run the container with host networking on Linux:

```bash
docker run --rm -it --network host -v lcemp-data:/data lcemp-server
```
