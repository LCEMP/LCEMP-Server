#!/bin/sh
set -eu

properties_file="${SERVER_PROPERTIES_FILE:-/data/server.properties}"

SERVER_PORT="${SERVER_PORT:-25565}"
SERVER_IP="${SERVER_IP:-}"
LEVEL_NAME="${LEVEL_NAME:-world}"
LEVEL_SEED="${LEVEL_SEED:-}"
LEVEL_SIZE="${LEVEL_SIZE:-large}"
GAMEMODE="${GAMEMODE:-0}"
DIFFICULTY="${DIFFICULTY:-2}"
MAX_PLAYERS="${MAX_PLAYERS:-8}"
PVP="${PVP:-true}"
TRUST_PLAYERS="${TRUST_PLAYERS:-true}"
FIRE_SPREADS="${FIRE_SPREADS:-true}"
TNT_EXPLODES="${TNT_EXPLODES:-true}"
STRUCTURES="${STRUCTURES:-true}"
SPAWN_ANIMALS="${SPAWN_ANIMALS:-true}"
SPAWN_NPCS="${SPAWN_NPCS:-true}"
ONLINE_MODE="${ONLINE_MODE:-false}"
SHOW_GAMERTAGS="${SHOW_GAMERTAGS:-true}"
MOTD="${MOTD:-A Minecraft LCE Server}"
WHITE_LIST="${WHITE_LIST:-false}"
VOICE_CHAT="${VOICE_CHAT:-false}"
ENABLE_CHAT="${ENABLE_CHAT:-true}"
ADVERTISE_LAN="${ADVERTISE_LAN:-true}"

write_property() {
    printf '%s=%s\n' "$1" "$2"
}

mkdir -p "$(dirname "$properties_file")"

{
    printf '#Minecraft server properties\n'
    printf '#Generated from container environment on %s\n' "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
    write_property server-port "$SERVER_PORT"
    write_property level-name "$LEVEL_NAME"
    write_property level-seed "$LEVEL_SEED"
    write_property gamemode "$GAMEMODE"
    write_property difficulty "$DIFFICULTY"
    write_property max-players "$MAX_PLAYERS"
    write_property pvp "$PVP"
    write_property trust-players "$TRUST_PLAYERS"
    write_property fire-spreads "$FIRE_SPREADS"
    write_property tnt-explodes "$TNT_EXPLODES"
    write_property structures "$STRUCTURES"
    write_property spawn-animals "$SPAWN_ANIMALS"
    write_property spawn-npcs "$SPAWN_NPCS"
    write_property online-mode "$ONLINE_MODE"
    write_property show-gamertags "$SHOW_GAMERTAGS"
    write_property motd "$MOTD"
    write_property white-list "$WHITE_LIST"
    write_property voice-chat "$VOICE_CHAT"
    write_property enable-chat "$ENABLE_CHAT"
    write_property level-size "$LEVEL_SIZE"
    write_property advertise-lan "$ADVERTISE_LAN"
    write_property server-ip "$SERVER_IP"
} > "$properties_file"

exec "$@"
