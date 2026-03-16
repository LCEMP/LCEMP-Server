#pragma once

#ifdef __linux__
#include "../../Minecraft.Server/Linux/PosixNetLayer.h"
#else
#error "This redirect header should only be used on Linux builds"
#endif
