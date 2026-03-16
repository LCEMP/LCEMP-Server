#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
SRC="$HOME/lcemp-src"
BUILD="$HOME/lcemp-build"
ACTION="${1:-Release}"

case "$ACTION" in
    Clean)
        rm -rf "$BUILD" "$SRC"
        echo "Cleaned."
        exit 0
        ;;
    Sync)
        rm -rf "$SRC"
        ;&
esac

if [ ! -d "$SRC/Minecraft.World" ]; then
    echo "copying source files to $SRC..."
    mkdir -p "$SRC"
    cp -a "$ROOT_DIR/." "$SRC/"
    rm -rf "$SRC/build_linux" "$SRC/build_dedicated" "$SRC/x64" "$SRC/x64_Server_Release" "$SRC/ipch" "$SRC/.vs"
else
    rsync -a "$ROOT_DIR/CMakeLists.txt" "$SRC/"
    rsync -a "$ROOT_DIR/cmake/" "$SRC/cmake/"
    rsync -a "$ROOT_DIR/Minecraft.Server/" "$SRC/Minecraft.Server/" --exclude='*.obj' --exclude='*.pdb' --exclude='*.sdf' --exclude='*.tlog'
    rsync -a "$ROOT_DIR/Minecraft.World/" "$SRC/Minecraft.World/" --exclude='*.obj' --exclude='*.pdb'
    rsync -a "$ROOT_DIR/Minecraft.Client/" "$SRC/Minecraft.Client/" --exclude='*.obj' --exclude='*.pdb'
fi

mkdir -p "$BUILD"
cd "$BUILD"
cmake "$SRC/Minecraft.Server" -DCMAKE_BUILD_TYPE=Release
cmake --build . --target MinecraftDedicatedServer -- -j$(nproc)
strip MinecraftDedicatedServer

echo ""
ls -lh MinecraftDedicatedServer
