#!/bin/sh
set -eu

image="${IMAGE:-lcemp-server:local}"
build_jobs="${BUILD_JOBS:-2}"
force_full_build="${FORCE_FULL_BUILD:-false}"
github_repo="${GITHUB_REPOSITORY:-}"
github_release_tag="${GITHUB_RELEASE_TAG:-latest}"
github_asset_name="${GITHUB_ASSET_NAME:-MinecraftDedicatedServer}"

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "$script_dir/../.." && pwd)"
prebuilt_binary="$repo_root/build/MinecraftDedicatedServer"

build_prebuilt_image() {
    echo "Using prebuilt Linux server binary: $prebuilt_binary"
    cd "$repo_root"
    docker build \
        -f Minecraft.Server/docker/Dockerfile.prebuilt \
        -t "$image" \
        .
}

build_full_image() {
    cd "$repo_root"
    docker build \
        -f Minecraft.Server/docker/Dockerfile \
        --build-arg BUILD_JOBS="$build_jobs" \
        -t "$image" \
        .
}

try_download_github_binary() {
    if [ -z "$github_repo" ]; then
        return 1
    fi

    if [ "$github_release_tag" = "latest" ] || [ -z "$github_release_tag" ]; then
        release_url="https://api.github.com/repos/$github_repo/releases/latest"
    else
        release_url="https://api.github.com/repos/$github_repo/releases/tags/$github_release_tag"
    fi

    if ! command -v python3 >/dev/null 2>&1; then
        echo "python3 is required to download GitHub release assets from build-image.sh."
        return 1
    fi

    echo "Looking for GitHub release asset '$github_asset_name' at $release_url"
    mkdir -p "$(dirname "$prebuilt_binary")"

    GITHUB_RELEASE_URL="$release_url" \
    GITHUB_ASSET_NAME="$github_asset_name" \
    PREBUILT_BINARY="$prebuilt_binary" \
    python3 - <<'PY'
import json
import os
import sys
import urllib.request

release_url = os.environ["GITHUB_RELEASE_URL"]
asset_name = os.environ["GITHUB_ASSET_NAME"]
prebuilt_binary = os.environ["PREBUILT_BINARY"]

headers = {
    "Accept": "application/vnd.github+json",
    "X-GitHub-Api-Version": "2022-11-28",
    "User-Agent": "lcemp-docker-build",
}

try:
    req = urllib.request.Request(release_url, headers=headers)
    with urllib.request.urlopen(req) as response:
        release = json.load(response)

    asset = next((item for item in release.get("assets", []) if item.get("name") == asset_name), None)
    if asset is None:
        print(f"GitHub release found, but asset '{asset_name}' was not present.")
        sys.exit(1)

    download_url = asset["browser_download_url"]
    req = urllib.request.Request(download_url, headers={"User-Agent": "lcemp-docker-build"})
    with urllib.request.urlopen(req) as response, open(prebuilt_binary, "wb") as output:
        output.write(response.read())

    print(f"Downloaded Linux server binary from GitHub: {download_url}")
except Exception as exc:
    print(f"GitHub binary download failed: {exc}")
    sys.exit(1)
PY
}

if [ "$force_full_build" != "true" ] && [ -f "$prebuilt_binary" ]; then
    build_prebuilt_image
else
    if [ "$force_full_build" = "true" ]; then
        echo "FORCE_FULL_BUILD=true; running full Docker build."
        build_full_image
    else
        echo "No prebuilt Linux server binary found at: $prebuilt_binary"
        if try_download_github_binary && [ -f "$prebuilt_binary" ]; then
            build_prebuilt_image
        else
            echo "Running full Docker build."
            build_full_image
        fi
    fi
fi
