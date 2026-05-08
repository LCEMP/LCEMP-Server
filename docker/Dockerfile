# syntax=docker/dockerfile:1

FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
ARG BUILD_JOBS=2

# Build with the LCEMP repository root as context:
# docker build -f Minecraft.Server/docker/Dockerfile .
COPY cmake ./cmake
COPY Minecraft.Client ./Minecraft.Client
COPY Minecraft.World ./Minecraft.World
COPY Minecraft.Server ./Minecraft.Server

RUN cmake -S Minecraft.Server -B /build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target MinecraftDedicatedServer --parallel "${BUILD_JOBS}" \
    && strip /build/MinecraftDedicatedServer

FROM debian:bookworm-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --create-home --home-dir /home/lcemp --shell /usr/sbin/nologin lcemp \
    && mkdir -p /data \
    && chown -R lcemp:lcemp /data

COPY --from=build /build/MinecraftDedicatedServer /usr/local/bin/MinecraftDedicatedServer
COPY Minecraft.Server/docker/entrypoint.sh /usr/local/bin/lcemp-entrypoint

RUN chmod +x /usr/local/bin/lcemp-entrypoint

WORKDIR /data
VOLUME ["/data"]

EXPOSE 25565/tcp
EXPOSE 25565/udp
EXPOSE 25566/udp

USER lcemp
ENTRYPOINT ["lcemp-entrypoint"]
CMD ["MinecraftDedicatedServer"]
