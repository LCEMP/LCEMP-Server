#include "stdafx.h"

#include "../../Minecraft.Client/Minecraft.h"
#include "../../Minecraft.Client/ProgressRenderer.h"
#include "../../Minecraft.Client/GameRenderer.h"
#include "../../Minecraft.Client/Textures.h"
#include "../../Minecraft.Client/Font.h"
#include "../../Minecraft.Client/Gui.h"
#include "../../Minecraft.Client/Options.h"
#include "../../Minecraft.Client/User.h"
#include "../../Minecraft.Client/StatsCounter.h"
#include "../../Minecraft.Client/LevelRenderer.h"
#include "../../Minecraft.Client/AchievementPopup.h"
#include "../../Minecraft.Client/KeyboardMouseInput.h"
#include "../../Minecraft.Client/Windows64/Sentient/SentientManager.h"
#include "../../Minecraft.Client/Windows64/Social/SocialManager.h"
#include "../../Minecraft.Client/StringTable.h"
#include "../../Minecraft.Client/MemoryTracker.h"

#include "../../Minecraft.Client/Windows64Media/strings.h"
#include <time.h>
#include "../Core/ServerLogger.h"

CRITICAL_SECTION ProgressRenderer::s_progress;

static const wchar_t* GetProgressTitleName(int id)
{
    switch(id)
    {
    case IDS_PROGRESS_INITIALISING_SERVER: return L"Starting integrated server";
    case IDS_PROGRESS_GENERATING_LEVEL:    return L"Generating level";
    case IDS_PROGRESS_GENERATING_SPAWN_AREA: return L"Preparing spawn area";
    case IDS_PROGRESS_LOADING_LEVEL:       return L"Loading level";
    case IDS_PROGRESS_LOADING_SPAWN_AREA:  return L"Loading spawn area";
    case IDS_PROGRESS_BUILDING_TERRAIN:    return L"Building terrain";
    case IDS_PROGRESS_SAVING_LEVEL:        return L"Saving level";
    case IDS_PROGRESS_SAVING_CHUNKS:       return L"Saving chunks";
    case IDS_PROGRESS_SAVING_PLAYERS:      return L"Saving players";
    case IDS_PROGRESS_SAVING_TO_DISC:      return L"Saving to disc";
    case IDS_PROGRESS_AUTOSAVING_LEVEL:    return L"Autosaving level";
    case IDS_PROGRESS_HOST_SAVING:         return L"Saving";
    case IDS_PROGRESS_RESPAWNING:          return L"Respawning";
    case IDS_PROGRESS_ENTERING_NETHER:     return L"Entering the Nether";
    case IDS_PROGRESS_LEAVING_NETHER:      return L"Leaving the Nether";
    case IDS_PROGRESS_ENTERING_END:        return L"Entering the End";
    case IDS_PROGRESS_LEAVING_END:         return L"Leaving the End";
    default: return NULL;
    }
}

ProgressRenderer::ProgressRenderer(Minecraft *minecraft) : minecraft(minecraft)
{
    lastPercent = 0;
    status = 0;
    title = 0;
    lastTime = 0;
    noAbort = false;
    m_eType = eProgressStringType_ID;
}

void ProgressRenderer::progressStart(int title)
{
    _progressStart(title);
}

void ProgressRenderer::progressStartNoAbort(int string)
{
    noAbort = true;
    _progressStart(string);
}

void ProgressRenderer::_progressStart(int t)
{
    this->title = t;
    this->lastPercent = -1;
    this->lastTime = 0;

    const wchar_t *name = GetProgressTitleName(t);
    if (name)
        ServerLog(L"%ls\n", name);
}

void ProgressRenderer::progressStage(int s)
{
    this->status = s;
}

void ProgressRenderer::progressStage(wstring &wstrText)
{
    m_wstrText = wstrText;
    m_eType = eProgressStringType_String;
}

void ProgressRenderer::progressStagePercentage(int i)
{
    if (i != lastPercent)
    {
        lastPercent = i;
        if (title == IDS_PROGRESS_GENERATING_SPAWN_AREA || title == IDS_PROGRESS_LOADING_SPAWN_AREA)
        {
            if (i % 10 == 0 && i > 0)
            {
                const wchar_t *name = GetProgressTitleName(title);
                if (name)
                    ServerLog(L"Preparing spawn area: %d%%%%\n", i);
            }
        }
    }
}

int ProgressRenderer::getCurrentPercent() { return lastPercent; }
int ProgressRenderer::getCurrentTitle() { return title; }
int ProgressRenderer::getCurrentStatus() { return status; }
wstring& ProgressRenderer::getProgressString(void) { return m_wstrText; }
ProgressRenderer::eProgressStringType ProgressRenderer::getType() { return m_eType; }
void ProgressRenderer::setType(eProgressStringType eType) { m_eType = eType; }

bool GameRenderer::anaglyph3d = false;
int GameRenderer::anaglyphPass = 0;

GameRenderer::GameRenderer(Minecraft *mc)
{
    // should be good enough
    this->mc = mc;
    renderDistance = 0;
    _tick = 0;
    hovered = nullptr;
    thirdDistance = 4;
    thirdDistanceO = 4;
    thirdRotation = 0;
    thirdRotationO = 0;
    thirdTilt = 0;
    thirdTiltO = 0;
    accumulatedSmoothXO = 0;
    accumulatedSmoothYO = 0;
    tickSmoothXO = 0;
    tickSmoothYO = 0;
    lastTickA = 0;
    cameraPos = NULL;
    fovOffset = 0;
    fovOffsetO = 0;
    cameraRoll = 0;
    cameraRollO = 0;
    isInClouds = false;
    zoom = 1;
    zoom_x = 0;
    zoom_y = 0;
    rainXa = NULL;
    rainZa = NULL;
    lastActiveTime = 0;
    lastNsTime = 0;
    random = NULL;
    rainSoundTime = 0;
    xMod = 0;
    yMod = 0;
    lb = NULL;
    fr = 0; fg = 0; fb = 0;
    fogBrO = 0; fogBr = 0;
    cameraFlip = 0;
    _updateLightTexture = false;
    blr = 0; blrt = 0; blg = 0;
    itemInHandRenderer = NULL;
    for (int i = 0; i < 4; i++) { fov[i] = 0; oFov[i] = 0; tFov[i] = 0; }
}

void GameRenderer::DisableUpdateThread()
{
}

void GameRenderer::EnableUpdateThread()
{
}

#ifdef MULTITHREAD_ENABLE
C4JThread* GameRenderer::m_updateThread = NULL;
C4JThread::EventArray* GameRenderer::m_updateEvents = NULL;
bool GameRenderer::nearThingsToDo = false;
bool GameRenderer::updateRunning = false;
#endif

vector<byte *> GameRenderer::m_deleteStackByte;
vector<SparseLightStorage *> GameRenderer::m_deleteStackSparseLightStorage;
vector<CompressedTileStorage *> GameRenderer::m_deleteStackCompressedTileStorage;
vector<SparseDataStorage *> GameRenderer::m_deleteStackSparseDataStorage;

void MemSect(int sect) {}

void glFlush() {}
void glTexGeni(int, int, int) {}
void glTexGen(int, int, FloatBuffer *) {}
void glReadPixels(int, int, int, int, int, int, ByteBuffer *) {}
void glClearDepth(double) {}
void glCullFace(int) {}
void glDeleteLists(int, int) {}

void PlayerRenderer_InitNametagColors() {}