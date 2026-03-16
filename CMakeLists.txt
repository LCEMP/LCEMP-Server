cmake_minimum_required(VERSION 3.10)
project("MinecraftDedicatedServer")

set(CMAKE_CXX_STANDARD 11)

if(NOT UNIX)
    message(FATAL_ERROR "The dedicated server currently only builds on Linux, use Visual Studio to build the Windows server")
endif()

set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

include("${ROOT_DIR}/cmake/Sources.cmake")
include("${ROOT_DIR}/cmake/ServerSources.cmake")

list(TRANSFORM MINECRAFT_WORLD_SOURCES PREPEND "${ROOT_DIR}/Minecraft.World/")
list(TRANSFORM MINECRAFT_SERVER_OWN_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM MINECRAFT_SERVER_SHARED_SOURCES PREPEND "${ROOT_DIR}/Minecraft.Client/")

set(BUNDLED_ZLIB_SOURCES
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/adler32.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/compress.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/crc32.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/deflate.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/inffast.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/inflate.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/inftrees.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/trees.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/uncompr.c"
    "${ROOT_DIR}/Minecraft.Client/Common/zlib/zutil.c"
)

add_library(MinecraftWorld STATIC ${MINECRAFT_WORLD_SOURCES} ${BUNDLED_ZLIB_SOURCES})

target_include_directories(MinecraftWorld PUBLIC
    "${ROOT_DIR}/Minecraft.World"
    "${ROOT_DIR}/Minecraft.World/x64headers"
)

target_include_directories(MinecraftWorld BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Linux/stubs"
)

target_include_directories(MinecraftWorld PRIVATE
    "${ROOT_DIR}/Minecraft.Client"
    "${ROOT_DIR}/Minecraft.Client/Windows64/Iggy/include"
    "${ROOT_DIR}/Minecraft.Client/Xbox/Sentient/Include"
)

target_compile_options(MinecraftWorld PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/Linux/LinuxCompat.h"
    -Wno-write-strings -Wno-narrowing -Wno-deprecated
    -ffunction-sections -fdata-sections
)

target_compile_definitions(MinecraftWorld PRIVATE
    _LARGE_WORLDS _LIB _WINDOWS64
    _CRT_NON_CONFORMING_SWPRINTFS _CRT_SECURE_NO_WARNINGS
)

add_executable(MinecraftDedicatedServer
    ${MINECRAFT_SERVER_OWN_SOURCES}
    ${MINECRAFT_SERVER_SHARED_SOURCES}
)

target_compile_definitions(MinecraftDedicatedServer PRIVATE
    _DEDICATED_SERVER
    _CONTENT_PACKAGE
    _LARGE_WORLDS
    _WINDOWS64
    WITH_SERVER_CODE
    _CRT_NON_CONFORMING_SWPRINTFS
    _CRT_SECURE_NO_WARNINGS
)

target_include_directories(MinecraftDedicatedServer BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Linux/stubs"
)

target_include_directories(MinecraftDedicatedServer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Linux"
    "${ROOT_DIR}/Minecraft.Client"
    "${ROOT_DIR}/Minecraft.Client/Windows64/4JLibs/inc"
    "${ROOT_DIR}/Minecraft.Client/Windows64/Iggy/include"
    "${ROOT_DIR}/Minecraft.Client/Xbox/Sentient/Include"
    "${ROOT_DIR}/Minecraft.World/x64headers"
)

target_compile_options(MinecraftDedicatedServer PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/Linux/LinuxCompat.h"
    -Wno-write-strings -Wno-narrowing -Wno-deprecated
    -ffunction-sections -fdata-sections
)

find_package(Threads REQUIRED)
target_link_libraries(MinecraftDedicatedServer PRIVATE
    MinecraftWorld
    Threads::Threads
)

target_link_options(MinecraftDedicatedServer PRIVATE
    -Wl,--gc-sections
)
