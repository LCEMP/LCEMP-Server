
#include "stdafx.h"

#include "../../Minecraft.World/Dimension.h"
#include "../../Minecraft.Client/Windows64/4JLibs/inc/4J_Input.h"
#include "../../Minecraft.Client/Windows64/4JLibs/inc/4J_Storage.h"
#include "../../Minecraft.Client/TexturePackRepository.h"
#include "../../Minecraft.Client/LevelRenderer.h"
#include "../../Minecraft.Client/Textures.h"
#include "../../Minecraft.Client/Font.h"
#include "../../Minecraft.Client/Gui.h"
#include "../../Minecraft.Client/ScreenSizeCalculator.h"
#include "../../Minecraft.Client/Timer.h"
#include "../../Minecraft.Client/HumanoidModel.h"
#include "../../Minecraft.Client/ParticleEngine.h"
#include "../../Minecraft.Client/Tesselator.h"
#include "../../Minecraft.Client/ItemInHandRenderer.h"
#include "../../Minecraft.Client/EntityRenderDispatcher.h"
#include "../../Minecraft.Client/TileEntityRenderDispatcher.h"
#include "../../Minecraft.Client/GuiParticles.h"
#include "../../Minecraft.Client/TitleScreen.h"
#include "../../Minecraft.Client/AchievementPopup.h"
#include "../../Minecraft.Client/Input.h"
#include "../../Minecraft.Client/TextureManager.h"
#include "../../Minecraft.Client/TileRenderer.h"
#include "../../Minecraft.Client/Chunk.h"
class EnderDragon;
#include "../../Minecraft.Client/EnderDragonRenderer.h"
#include "../../Minecraft.Client/GameRenderer.h"
#include "../../Minecraft.Client/GameMode.h"
#include "../../Minecraft.Client/KeyMapping.h"
#include "../../Minecraft.Client/ArchiveFile.h"
#include "../../Minecraft.Client/BufferedImage.h"
#include "../../Minecraft.Client/Lighting.h"
#include "../../Minecraft.Client/Model.h"
#include "../../Minecraft.Client/OffsettedRenderList.h"
#include "../../Minecraft.Client/Common/Audio/SoundEngine.h"
#include "../../Minecraft.Client/Common/UI/UIController.h"
#include "../../Minecraft.Client/Windows64/Windows64_UIController.h"
#include "../../Minecraft.Client/Common/UI/UIControl_Progress.h"
#include "../../Minecraft.Client/Common/UI/UIControl_Label.h"
#include "../../Minecraft.Client/Common/UI/UIControl_Button.h"
#include "../../Minecraft.Client/Common/UI/UIScene_FullscreenProgress.h"
#include "../../Minecraft.Client/Common/UI/IUIScene_PauseMenu.h"
#include "../../Minecraft.Client/Common/UI/IUIScene_CreativeMenu.h"
#include "../../Minecraft.Client/Common/UI/IUIScene_TradingMenu.h"
#include "../../Minecraft.Client/Common/UI/IUIScene_CraftingMenu.h"
#include "../../Minecraft.Client/Common/UI/UIFontData.h"
#include "../../Minecraft.Client/Particle.h"
#include "../../Minecraft.Client/CritParticle.h"
#include "../../Minecraft.Client/TakeAnimationParticle.h"
#include "../../Minecraft.Client/Screen.h"
#include "../../Minecraft.Client/GuiComponent.h"

C_4JInput InputManager;
wchar_t g_Win64UsernameW[17] = L"Server";
char g_Win64Username[17] = "Server";
HWND g_hWnd = NULL;

bool TileRenderer::fancy = false;
EntityRenderDispatcher *EntityRenderDispatcher::instance = NULL;
TileEntityRenderDispatcher *TileEntityRenderDispatcher::instance = NULL;
int Chunk::updates = 0;
LevelRenderer *Chunk::levelRenderer = NULL;
shared_ptr<EnderDragon> EnderDragonRenderer::bossInstance;
int EnderDragonRenderer::currentModel = 0;

double EntityRenderDispatcher::xOff = 0;
double EntityRenderDispatcher::yOff = 0;
double EntityRenderDispatcher::zOff = 0;

double TileEntityRenderDispatcher::xOff = 0;
double TileEntityRenderDispatcher::yOff = 0;
double TileEntityRenderDispatcher::zOff = 0;

bool Textures::MIPMAP = false;
C4JRender::eTextureFormat Textures::TEXTURE_FORMAT = C4JRender::TEXTURE_FORMAT_RxGyBzAw;

float Gui::currentGuiBlendFactor = 1.0f;
float Gui::currentGuiScaleFactor = 1.0f;

const int LevelRenderer::MAX_LEVEL_RENDER_SIZE[3] = { 356, 42, 18 };
const int LevelRenderer::DIMENSION_OFFSETS[3] = { 0, (356 * 356 * 16), (356 * 356 * 16) + (42 * 42 * 16) };

int ItemInHandRenderer::list = 0;
int ItemInHandRenderer::listGlint = 0;

TexturePack *TexturePackRepository::DEFAULT_TEXTURE_PACK = NULL;

FloatBuffer *Lighting::lb = NULL;

void C_4JInput::Tick() {}
unsigned int C_4JInput::GetGameJoypadMaps(unsigned char, unsigned char) { return 0; }
unsigned char C_4JInput::GetJoypadMapVal(int) { return 0; }
void C_4JInput::SetJoypadMapVal(int, unsigned char) {}
unsigned int C_4JInput::GetValue(int, unsigned char, bool) { return 0; }
bool C_4JInput::ButtonPressed(int, unsigned char) { return false; }
bool C_4JInput::ButtonReleased(int, unsigned char) { return false; }
bool C_4JInput::ButtonDown(int, unsigned char) { return false; }
bool C_4JInput::IsPadConnected(int) { return false; }
float C_4JInput::GetJoypadStick_LX(int, bool) { return 0.0f; }
float C_4JInput::GetJoypadStick_LY(int, bool) { return 0.0f; }
float C_4JInput::GetJoypadStick_RX(int, bool) { return 0.0f; }
float C_4JInput::GetJoypadStick_RY(int, bool) { return 0.0f; }
float C_4JInput::GetIdleSeconds(int) { return 0.0f; }
void C_4JInput::SetJoypadStickAxisMap(int, unsigned int, unsigned int) {}
void C_4JInput::SetJoypadStickTriggerMap(int, unsigned int, unsigned int) {}
void C_4JInput::SetDebugSequence(const char *, int (*)(LPVOID), LPVOID) {}
void C_4JInput::SetMenuDisplayed(int, bool) {}

void glTranslatef(float, float, float) {}
void glRotatef(float, float, float, float) {}
void glScalef(float, float, float) {}
void glPushMatrix() {}
void glPopMatrix() {}
void glEnable(int) {}
void glDisable(int) {}
void glDepthFunc(int) {}
void glMatrixMode(int) {}
void glLoadIdentity() {}
void glBindTexture(int, int) {}
void glShadeModel(int) {}
void glLineWidth(float) {}
void glClear(int) {}
void glViewport(int, int, int, int) {}
void glAlphaFunc(int, float) {}
void glOrtho(float, float, float, float, float, float) {}
void glClearColor(float, float, float, float) {}
void glBlendFunc(int, int) {}
void glDeleteTextures(int) {}
void glDeleteTextures(IntBuffer *) {}
void glDepthMask(bool) {}
void glColor4f(float, float, float, float) {}
void glColor3f(float, float, float) {}
void glNormal3f(float, float, float) {}
void glNewList(int, int) {}
void glEndList(int) {}
void glCallList(int) {}
void glCallLists(IntBuffer *) {}
void glDrawArrays(int, int, int) {}
void glTexCoordPointer(int, int, int, int) {}
void glTexCoordPointer(int, int, FloatBuffer *) {}
void glNormalPointer(int, int, int) {}
void glNormalPointer(int, ByteBuffer *) {}
void glEnableClientState(int) {}
void glDisableClientState(int) {}
void glColorPointer(int, bool, int, ByteBuffer *) {}
void glColorPointer(int, int, int, int) {}
void glVertexPointer(int, int, int, int) {}
void glVertexPointer(int, int, FloatBuffer *) {}
void glMultMatrixf(float *) {}
void glTexParameteri(int, int, int) {}
int glGenTextures() { return 0; }
int glGenLists(int) { return 0; }
void glTexImage2D(int, int, int, int, int, int, int, int, ByteBuffer *) {}
void glColorMask(bool, bool, bool, bool) {}
void glGenQueriesARB(IntBuffer *) {}
void glBeginQueryARB(int, int) {}
void glEndQueryARB(int) {}
void glGetQueryObjectuARB(int, int, IntBuffer *) {}
void glPolygonOffset(float, float) {}
void glScaled(double, double, double) {}
void gluPerspective(float, float, float, float) {}
void glFogi(int, int) {}
void glFogf(int, float) {}
void glFog(int, FloatBuffer *) {}
void glColorMaterial(int, int) {}
void glMultiTexCoord2f(int, float, float) {}
void glClientActiveTexture(int) {}
void glActiveTexture(int) {}

void Display::update() {}
void Display::swapBuffers() {}

Textures::Textures(TexturePackRepository *, Options *) {}
int Textures::loadTexture(int) { return 0; }
void Textures::tick(bool, bool) {}
void Textures::reloadAll() {}
void Textures::stitch() {}
void Textures::bind(int) {}
int Textures::getTexture(BufferedImage *, C4JRender::eTextureFormat, bool) { return 0; }
void Textures::replaceTextureDirect(intArray, int, int, int) {}
void Textures::releaseTexture(int) {}
intArray Textures::loadTexturePixels(TEXTURE_NAME, const wstring &) { return intArray(); }
void Textures::bindTexture(const wstring &) {}
void Textures::bindTexture(int) {}
void Textures::clearLastBoundId() {}
void Textures::loadTexture(BufferedImage *, int) {}
void Textures::loadTexture(BufferedImage *, int, bool, bool) {}
void Textures::replaceTexture(intArray, int, int, int) {}
void Textures::replaceTextureDirect(shortArray, int, int, int) {}
int Textures::loadHttpTexture(const wstring &, const wstring &) { return 0; }
int Textures::loadHttpTexture(const wstring &, int) { return 0; }
bool Textures::hasHttpTexture(const wstring &) { return false; }
HttpTexture *Textures::addHttpTexture(const wstring &, HttpTextureProcessor *) { return NULL; }
void Textures::removeHttpTexture(const wstring &) {}
int Textures::loadMemTexture(const wstring &, const wstring &) { return 0; }
int Textures::loadMemTexture(const wstring &, int) { return 0; }
MemTexture *Textures::addMemTexture(const wstring &, MemTextureProcessor *) { return NULL; }
void Textures::removeMemTexture(const wstring &) {}
Icon *Textures::getMissingIcon(int) { return NULL; }
BufferedImage *Textures::readImage(TEXTURE_NAME, const wstring &) { return NULL; }
bool Textures::IsTUImage(TEXTURE_NAME, const wstring &) { return false; }
bool Textures::IsOriginalImage(TEXTURE_NAME, const wstring &) { return false; }

Font::Font(Options *, const wstring &, Textures *, bool, TEXTURE_NAME, int, int, int, int, unsigned short *) {}
#ifndef _XBOX
Font::~Font() {}
#endif
void Font::drawShadow(const wstring &, int, int, int) {}
void Font::drawShadowWordWrap(const wstring &, int, int, int, int, int) {}
void Font::draw(const wstring &, int, int, int) {}
void Font::renderFakeCB(IntBuffer *) {}

Gui::Gui(Minecraft *) {}
void Gui::render(float, bool, int, int) {}
void Gui::tick() {}
void Gui::clearMessages(int) {}
void Gui::addMessage(const wstring &, int, bool) {}
void Gui::setNowPlaying(const wstring &) {}
void Gui::displayClientMessage(int, int) {}
float Gui::getOpacity(int, DWORD) { return 0.0f; }

ScreenSizeCalculator::ScreenSizeCalculator(Options *, int, int, int) {}
int ScreenSizeCalculator::getWidth() { return 800; }
int ScreenSizeCalculator::getHeight() { return 600; }

Timer::Timer(float) {}
void Timer::advanceTime() {}
void Timer::advanceTimeQuickly() {}
void Timer::skipTime() {}

LevelRenderer::LevelRenderer(Minecraft *, Textures *) {}
void LevelRenderer::setLevel(int, MultiPlayerLevel *) {}
void LevelRenderer::allChanged() {}
void LevelRenderer::allChanged(int) {}
void LevelRenderer::tick() {}
void LevelRenderer::clear() {}
int LevelRenderer::render(shared_ptr<Mob>, int, double, bool) { return 0; }
void LevelRenderer::renderEntities(Vec3 *, Culler *, float) {}
void LevelRenderer::renderSky(float) {}
void LevelRenderer::renderClouds(float) {}
bool LevelRenderer::updateDirtyChunks() { return false; }
void LevelRenderer::renderHit(shared_ptr<Player>, HitResult *, int, shared_ptr<ItemInstance>, float) {}
void LevelRenderer::renderDestroyAnimation(Tesselator *, shared_ptr<Player>, float) {}
void LevelRenderer::renderHitOutline(shared_ptr<Player>, HitResult *, int, shared_ptr<ItemInstance>, float) {}
wstring LevelRenderer::gatherStats1() { return L""; }
wstring LevelRenderer::gatherStats2() { return L""; }
void LevelRenderer::setDirty(int, int, int, int, int, int, Level *) {}
void LevelRenderer::tileChanged(int, int, int) {}
void LevelRenderer::tileLightChanged(int, int, int) {}
void LevelRenderer::setTilesDirty(int, int, int, int, int, int, Level *) {}
void LevelRenderer::cull(Culler *, float) {}
void LevelRenderer::playStreamingMusic(const wstring &, int, int, int) {}
void LevelRenderer::playSound(int, double, double, double, float, float, float) {}
void LevelRenderer::playSound(shared_ptr<Entity>, int, double, double, double, float, float, float) {}
void LevelRenderer::addParticle(ePARTICLE_TYPE, double, double, double, double, double, double) {}
shared_ptr<Particle> LevelRenderer::addParticleInternal(ePARTICLE_TYPE, double, double, double, double, double, double) { return nullptr; }
void LevelRenderer::entityAdded(shared_ptr<Entity>) {}
void LevelRenderer::entityRemoved(shared_ptr<Entity>) {}
void LevelRenderer::skyColorChanged() {}
void LevelRenderer::levelEvent(shared_ptr<Player>, int, int, int, int, int) {}
void LevelRenderer::destroyTileProgress(int, int, int, int, int) {}
void LevelRenderer::registerTextures(IconRegister *) {}
void LevelRenderer::render(AABB *) {}
int LevelRenderer::activePlayers() { return 0; }
void LevelRenderer::renderSameAsLast(int, double) {}
void LevelRenderer::renderHaloRing(float) {}
bool LevelRenderer::isInCloud(double, double, double, float) { return false; }
void LevelRenderer::renderAdvancedClouds(float) {}
void LevelRenderer::AddDLCSkinsToMemTextures() {}
void LevelRenderer::fullyFlagRenderableTileEntitiesToBeRemoved() {}

int LevelRenderer::getDimensionIndexFromId(int id) { return (3 - id) % 3; }

int LevelRenderer::getGlobalIndexForChunk(int x, int y, int z, Level *level) {
	return getGlobalIndexForChunk(x, y, z, level->dimension->id);
}
int LevelRenderer::getGlobalIndexForChunk(int x, int y, int z, int dimensionId) {
	int dimIdx = getDimensionIndexFromId(dimensionId);
	int xx = (x / CHUNK_XZSIZE) + (MAX_LEVEL_RENDER_SIZE[dimIdx] / 2);
	int yy = y / CHUNK_SIZE;
	int zz = (z / CHUNK_XZSIZE) + (MAX_LEVEL_RENDER_SIZE[dimIdx] / 2);
	if ((xx < 0) || (xx >= MAX_LEVEL_RENDER_SIZE[dimIdx])) return -1;
	if ((zz < 0) || (zz >= MAX_LEVEL_RENDER_SIZE[dimIdx])) return -1;
	if ((yy < 0) || (yy >= CHUNK_Y_COUNT)) return -1;
	int offset = DIMENSION_OFFSETS[dimIdx];
	offset += (zz * MAX_LEVEL_RENDER_SIZE[dimIdx] + xx) * CHUNK_Y_COUNT;
	offset += yy;
	return offset;
}
bool LevelRenderer::isGlobalIndexInSameDimension(int idx, Level *level) {
	int dim = getDimensionIndexFromId(level->dimension->id);
	int idxDim = 0;
	if (idx >= DIMENSION_OFFSETS[2]) idxDim = 2;
	else if (idx >= DIMENSION_OFFSETS[1]) idxDim = 1;
	return (dim == idxDim);
}
int LevelRenderer::getGlobalChunkCount() {
	return (MAX_LEVEL_RENDER_SIZE[0] * MAX_LEVEL_RENDER_SIZE[0] * CHUNK_Y_COUNT) +
		   (MAX_LEVEL_RENDER_SIZE[1] * MAX_LEVEL_RENDER_SIZE[1] * CHUNK_Y_COUNT) +
		   (MAX_LEVEL_RENDER_SIZE[2] * MAX_LEVEL_RENDER_SIZE[2] * CHUNK_Y_COUNT);
}
int LevelRenderer::getGlobalChunkCountForOverworld() {
	return (MAX_LEVEL_RENDER_SIZE[0] * MAX_LEVEL_RENDER_SIZE[0] * CHUNK_Y_COUNT);
}
bool LevelRenderer::getGlobalChunkFlag(int, int, int, Level *, unsigned char, unsigned char) { return false; }
void LevelRenderer::setGlobalChunkFlag(int, int, int, Level *, unsigned char, unsigned char) {}
void LevelRenderer::setGlobalChunkFlag(int, unsigned char, unsigned char) {}
void LevelRenderer::clearGlobalChunkFlag(int, int, int, Level *, unsigned char, unsigned char) {}

LevelRenderer::DestroyedTileManager::DestroyedTileManager() {}
LevelRenderer::DestroyedTileManager::~DestroyedTileManager() {}
void LevelRenderer::DestroyedTileManager::destroyingTileAt(Level *, int, int, int) {}
void LevelRenderer::DestroyedTileManager::updatedChunkAt(Level *, int, int, int, int) {}
void LevelRenderer::DestroyedTileManager::addAABBs(Level *, AABB *, AABBList *) {}
void LevelRenderer::DestroyedTileManager::tick() {}

ParticleEngine::ParticleEngine(Level *, Textures *) {}
ParticleEngine::~ParticleEngine() {}
void ParticleEngine::add(shared_ptr<Particle>) {}
void ParticleEngine::tick() {}
void ParticleEngine::render(shared_ptr<Entity>, float) {}
void ParticleEngine::renderLit(shared_ptr<Entity>, float) {}
void ParticleEngine::setLevel(Level *) {}
void ParticleEngine::destroy(int, int, int, int, int) {}
void ParticleEngine::crack(int, int, int, int) {}
wstring ParticleEngine::countParticles() { return L"0"; }

void GameRenderer::tick(bool) {}
void GameRenderer::pick(float) {}
void GameRenderer::render(float, bool) {}
void GameRenderer::AddForDelete(SparseLightStorage *) {}
void GameRenderer::AddForDelete(CompressedTileStorage *) {}
void GameRenderer::AddForDelete(SparseDataStorage *) {}
void GameRenderer::FinishedReassigning() {}

Tesselator *Tesselator::getInstance()
{
	static char buffer[4096] = {};
	return reinterpret_cast<Tesselator *>(buffer);
}
void Tesselator::CreateNewThreadStorage(int) {}
void Tesselator::end() {}
void Tesselator::begin() {}
void Tesselator::begin(int) {}
void Tesselator::useCompactVertices(bool) {}
bool Tesselator::getCompactVertices() { return false; }
void Tesselator::useProjectedTexture(bool) {}
void Tesselator::tex(float, float) {}
void Tesselator::tex2(int) {}
void Tesselator::color(float, float, float) {}
void Tesselator::color(float, float, float, float) {}
void Tesselator::color(int, int, int) {}
void Tesselator::color(int, int, int, int) {}
void Tesselator::color(byte, byte, byte) {}
void Tesselator::vertexUV(float, float, float, float, float) {}
void Tesselator::vertex(float, float, float) {}
void Tesselator::color(int) {}
void Tesselator::color(int, int) {}
void Tesselator::noColor() {}
void Tesselator::normal(float, float, float) {}
void Tesselator::offset(float, float, float) {}
void Tesselator::addOffset(float, float, float) {}
bool Tesselator::setMipmapEnable(bool) { return false; }
bool Tesselator::hasMaxVertices() { return false; }

HumanoidModel::HumanoidModel() {}
HumanoidModel::HumanoidModel(float) {}
HumanoidModel::HumanoidModel(float, float, int, int) {}

ItemInHandRenderer::ItemInHandRenderer(Minecraft *, bool) {}
void ItemInHandRenderer::itemPlaced() {}
void ItemInHandRenderer::itemUsed() {}
void ItemInHandRenderer::render(float) {}
void ItemInHandRenderer::tick() {}

void EntityRenderDispatcher::staticCtor() {}
EntityRenderer *EntityRenderDispatcher::getRenderer(eINSTANCEOF) { return NULL; }
EntityRenderer *EntityRenderDispatcher::getRenderer(shared_ptr<Entity>) { return NULL; }
void EntityRenderDispatcher::prepare(Level *, Textures *, Font *, shared_ptr<Mob>, Options *, float) {}
void EntityRenderDispatcher::render(shared_ptr<Entity>, float) {}
void EntityRenderDispatcher::render(shared_ptr<Entity>, double, double, double, float, float, bool, bool) {}
void EntityRenderDispatcher::setLevel(Level *) {}
double EntityRenderDispatcher::distanceToSqr(double, double, double) { return 0; }
Font *EntityRenderDispatcher::getFont() { return NULL; }
void EntityRenderDispatcher::registerTerrainTextures(IconRegister *) {}

void TileEntityRenderDispatcher::staticCtor() {}
TileEntityRenderer *TileEntityRenderDispatcher::getRenderer(eINSTANCEOF) { return NULL; }
bool TileEntityRenderDispatcher::hasRenderer(shared_ptr<TileEntity>) { return false; }
TileEntityRenderer *TileEntityRenderDispatcher::getRenderer(shared_ptr<TileEntity>) { return NULL; }
void TileEntityRenderDispatcher::prepare(Level *, Textures *, Font *, shared_ptr<Mob>, float) {}
void TileEntityRenderDispatcher::render(shared_ptr<TileEntity>, float, bool) {}
void TileEntityRenderDispatcher::render(shared_ptr<TileEntity>, double, double, double, float, bool, float, bool) {}
void TileEntityRenderDispatcher::setLevel(Level *) {}
double TileEntityRenderDispatcher::distanceToSqr(double, double, double) { return 0; }
Font *TileEntityRenderDispatcher::getFont() { return NULL; }

GuiParticles::GuiParticles(Minecraft *) {}
void GuiParticles::tick() {}
void GuiParticles::render(float) {}

TitleScreen::TitleScreen() {}

AchievementPopup::AchievementPopup(Minecraft *) {}
void AchievementPopup::render() {}

Input::Input() : xa(0), ya(0), wasJumping(false), jumping(false), sneaking(false), sprinting(false) {}
void Input::tick(LocalPlayer *) {}

void TextureManager::createInstance() {}
TextureManager *TextureManager::getInstance() { return NULL; }

void Lighting::turnOff() {}
void Lighting::turnOn() {}
void Lighting::turnOnGui() {}

TexturePackRepository::TexturePackRepository(File, Minecraft *) {}
void TexturePackRepository::addDebugPacks() {}
TexturePack *TexturePackRepository::getSelected() { return NULL; }
bool TexturePackRepository::selectTexturePackById(DWORD) { return false; }
TexturePack *TexturePackRepository::addTexturePackFromDLC(DLCPack *, DWORD) { return NULL; }
void TexturePackRepository::updateUI() {}
bool TexturePackRepository::needsUIUpdate() { return false; }
unsigned int TexturePackRepository::getTexturePackCount() { return 0; }
TexturePack *TexturePackRepository::getTexturePackByIndex(unsigned int) { return NULL; }
unsigned int TexturePackRepository::getTexturePackIndex(unsigned int) { return 0; }
TexturePack *TexturePackRepository::getTexturePackById(DWORD) { return NULL; }
void TexturePackRepository::clearInvalidTexturePacks() {}
void TexturePackRepository::updateList() {}
vector<TexturePack *> *TexturePackRepository::getAll() { return NULL; }
bool TexturePackRepository::selectSkin(TexturePack *) { return false; }

ArchiveFile::ArchiveFile(File) {}
ArchiveFile::~ArchiveFile() {}
bool ArchiveFile::hasFile(const wstring &) { return false; }
int ArchiveFile::getFileSize(const wstring &) { return 0; }
byteArray ArchiveFile::getFile(const wstring &) { return byteArray(); }
vector<wstring> *ArchiveFile::getFileList() { return NULL; }

KeyMapping::KeyMapping(const wstring &, int) {}

GameMode::GameMode(Minecraft *mc) : minecraft(mc), instaBuild(false) {}
void GameMode::initLevel(Level *) {}
bool GameMode::destroyBlock(int, int, int, int) { return false; }
void GameMode::render(float) {}
void GameMode::initPlayer(shared_ptr<Player>) {}
void GameMode::tick() {}
void GameMode::adjustPlayer(shared_ptr<Player>) {}
bool GameMode::useItem(shared_ptr<Player>, Level *, shared_ptr<ItemInstance>, bool) { return false; }
shared_ptr<Player> GameMode::createPlayer(Level *) { return nullptr; }
bool GameMode::interact(shared_ptr<Player>, shared_ptr<Entity>) { return false; }
void GameMode::attack(shared_ptr<Player>, shared_ptr<Entity>) {}
shared_ptr<ItemInstance> GameMode::handleInventoryMouseClick(int, int, int, bool, shared_ptr<Player>) { return nullptr; }
void GameMode::handleCloseInventory(int, shared_ptr<Player>) {}
void GameMode::handleInventoryButtonClick(int, int) {}
bool GameMode::isCutScene() { return false; }
void GameMode::releaseUsingItem(shared_ptr<Player>) {}
bool GameMode::hasExperience() { return false; }
bool GameMode::hasMissTime() { return false; }
bool GameMode::hasInfiniteItems() { return false; }
bool GameMode::hasFarPickRange() { return false; }
void GameMode::handleCreativeModeItemAdd(shared_ptr<ItemInstance>, int) {}
void GameMode::handleCreativeModeItemDrop(shared_ptr<ItemInstance>) {}
bool GameMode::handleCraftItem(int, shared_ptr<Player>) { return false; }
void GameMode::handleDebugOptions(unsigned int, shared_ptr<Player>) {}

BufferedImage::BufferedImage(int, int, int) { memset(data, 0, sizeof(data)); width = 0; height = 0; }
BufferedImage::BufferedImage(const wstring &, bool, bool, const wstring &) { memset(data, 0, sizeof(data)); width = 0; height = 0; }
BufferedImage::BufferedImage(DLCPack *, const wstring &, bool) { memset(data, 0, sizeof(data)); width = 0; height = 0; }
BufferedImage::BufferedImage(BYTE *, DWORD) { memset(data, 0, sizeof(data)); width = 0; height = 0; }
BufferedImage::~BufferedImage() {}
int BufferedImage::getWidth() { return width; }
int BufferedImage::getHeight() { return height; }
void BufferedImage::getRGB(int, int, int, int, intArray, int, int, int) {}
int *BufferedImage::getData() { return NULL; }
int *BufferedImage::getData(int) { return NULL; }
Graphics *BufferedImage::getGraphics() { return NULL; }
int BufferedImage::getTransparency() { return 0; }
BufferedImage *BufferedImage::getSubimage(int, int, int, int) { return NULL; }
void BufferedImage::preMultiplyAlpha() {}

void Windows64_UpdateGamma(unsigned short) {}

__int64 UIController::iggyAllocCount = 0;
CRITICAL_SECTION UIController::ms_reloadSkinCS;
bool UIController::ms_bReloadSkinCSInitialised = false;
DWORD UIController::m_dwTrialTimerLimitSecs = 0;

UIController::UIController()
{
	m_uiDebugConsole = NULL;
	m_uiDebugMarketingGuide = NULL;
	memset(&m_navigationLock, 0, sizeof(m_navigationLock));
	memset(m_actionRepeatTimer, 0, sizeof(m_actionRepeatTimer));
	m_fScreenWidth = 0;
	m_fScreenHeight = 0;
	m_bScreenWidthSetup = false;
	m_tileOriginX = 0;
	m_tileOriginY = 0;
	m_mcBitmapFont = NULL;
	m_mcTTFFont = NULL;
	m_moj7 = NULL;
	m_moj11 = NULL;
	gdraw_funcs = NULL;
	iggy_explorer = NULL;
	iggy_perfmon = NULL;
	m_iggyPerfmonEnabled = false;
	memset(m_bMenuDisplayed, 0, sizeof(m_bMenuDisplayed));
	memset(m_bMenuToBeClosed, 0, sizeof(m_bMenuToBeClosed));
	memset(m_iCountDown, 0, sizeof(m_iCountDown));
	memset(m_bCloseAllScenes, 0, sizeof(m_bCloseAllScenes));
	m_iPressStartQuadrantsMask = 0;
	m_bCustomRenderPosition = false;
	m_winUserIndex = 0;
	m_bSystemUIShowing = false;
	m_reloadSkinThread = NULL;
	m_navigateToHomeOnReload = false;
	m_accumulatedTicks = 0;
	memset(&m_customRenderingClearRect, 0, sizeof(m_customRenderingClearRect));
	memset(&m_registeredCallbackScenesCS, 0, sizeof(m_registeredCallbackScenesCS));
	for (int i = 0; i < eUIGroup_COUNT; i++) m_groups[i] = NULL;
	memset(&m_Allocatorlock, 0, sizeof(m_Allocatorlock));
	m_defaultBuffer = NULL;
	m_tempBuffer = NULL;
}

void UIController::tick() {}
void UIController::StartReloadSkinThread() {}
bool UIController::IsReloadingSkin() { return false; }
bool UIController::IsExpectingOrReloadingSkin() { return false; }
void UIController::CleanUpSkinReload() {}
bool UIController::NavigateToScene(int, EUIScene, void *, EUILayer, EUIGroup) { return false; }
bool UIController::NavigateBack(int, bool, EUIScene, EUILayer) { return false; }
void UIController::NavigateToHomeMenu() {}
UIScene *UIController::GetTopScene(int, EUILayer, EUIGroup) { return NULL; }
void UIController::CloseUIScenes(int, bool) {}
void UIController::CloseAllPlayersScenes() {}
bool UIController::IsPauseMenuDisplayed(int) { return false; }
bool UIController::IsContainerMenuDisplayed(int) { return false; }
bool UIController::IsIgnorePlayerJoinMenuDisplayed(int) { return false; }
bool UIController::IsIgnoreAutosaveMenuDisplayed(int) { return false; }
void UIController::SetIgnoreAutosaveMenuDisplayed(int, bool) {}
bool UIController::IsSceneInStack(int, EUIScene) { return false; }
void UIController::CheckMenuDisplayed() {}
void UIController::AnimateKeyPress(int, int, bool, bool, bool) {}
void UIController::OverrideSFX(int, int, bool) {}
void UIController::SetTooltipText(unsigned int, unsigned int, int) {}
void UIController::SetEnableTooltips(unsigned int, BOOL) {}
void UIController::ShowTooltip(unsigned int, unsigned int, bool) {}
void UIController::SetTooltips(unsigned int, int, int, int, int, int, int, int, int, int, bool) {}
void UIController::EnableTooltip(unsigned int, unsigned int, bool) {}
void UIController::RefreshTooltips(unsigned int) {}
void UIController::PlayUISFX(ESoundEffect) {}
void UIController::DisplayGamertag(unsigned int, bool) {}
void UIController::SetSelectedItem(unsigned int, const wstring &) {}
void UIController::UpdateSelectedItemPos(unsigned int) {}
void UIController::HandleDLCMountingComplete() {}
void UIController::HandleDLCInstalled(int) {}
void UIController::HandleTMSDLCFileRetrieved(int) {}
void UIController::HandleTMSBanFileRetrieved(int) {}
void UIController::HandleInventoryUpdated(int) {}
void UIController::HandleGameTick() {}
void UIController::SetTutorial(int, Tutorial *) {}
void UIController::SetTutorialDescription(int, TutorialPopupInfo *) {}
void UIController::RemoveInteractSceneReference(int, UIScene *) {}
void UIController::SetTutorialVisible(int, bool) {}
bool UIController::IsTutorialVisible(int) { return false; }
void UIController::UpdatePlayerBasePositions() {}
void UIController::SetEmptyQuadrantLogo(int) {}
void UIController::HideAllGameUIElements() {}
void UIController::ShowOtherPlayersBaseScene(unsigned int, bool) {}
void UIController::ShowTrialTimer(bool) {}
void UIController::SetTrialTimerLimitSecs(unsigned int) {}
void UIController::UpdateTrialTimer(unsigned int) {}
void UIController::ReduceTrialTimerValue() {}
void UIController::ShowAutosaveCountdownTimer(bool) {}
void UIController::UpdateAutosaveCountdownTimer(unsigned int) {}
void UIController::ShowSavingMessage(unsigned int, C4JStorage::ESavingMessage) {}
void UIController::ShowPlayerDisplayname(bool) {}
bool UIController::PressStartPlaying(unsigned int) { return false; }
void UIController::ShowPressStart(unsigned int) {}
void UIController::HidePressStart() {}
void UIController::ClearPressStart() {}
void UIController::SetWinUserIndex(unsigned int) {}
unsigned int UIController::GetWinUserIndex() { return 0; }
bool UIController::GetMenuDisplayed(int) { return false; }
void UIController::SetMenuDisplayed(int, bool) {}
void UIController::SetupFont() {}
void UIController::ReloadSkin() {}
byteArray UIController::getMovieData(const wstring &) { return byteArray(); }
void UIController::getRenderDimensions(C4JRender::eViewportType, S32 &, S32 &) {}
void UIController::setupRenderPosition(C4JRender::eViewportType) {}
void UIController::setupRenderPosition(S32, S32) {}
void UIController::SetSysUIShowing(bool) {}
void UIController::SetSystemUIShowing(LPVOID, bool) {}
void UIController::setupCustomDrawGameState() {}
void UIController::endCustomDrawGameState() {}
void UIController::setupCustomDrawMatrices(UIScene *, CustomDrawData *) {}
void UIController::setupCustomDrawGameStateAndMatrices(UIScene *, CustomDrawData *) {}
void UIController::endCustomDrawMatrices() {}
void UIController::endCustomDrawGameStateAndMatrices() {}
void UIController::registerSubstitutionTexture(const wstring &, PBYTE, DWORD) {}
void UIController::unregisterSubstitutionTexture(const wstring &, bool) {}
size_t UIController::RegisterForCallbackId(UIScene *) { return 0; }
void UIController::UnregisterCallbackId(size_t) {}
UIScene *UIController::GetSceneFromCallbackId(size_t) { return NULL; }
void UIController::EnterCallbackIdCriticalSection() {}
void UIController::LeaveCallbackIdCriticalSection() {}
#ifndef __PS3__
C4JStorage::EMessageResult UIController::RequestMessageBox(UINT, UINT, UINT *, UINT, DWORD, int (*)(LPVOID, int, const C4JStorage::EMessageResult), LPVOID, C4JStringTable *, WCHAR *, DWORD, bool) { return (C4JStorage::EMessageResult)0; }
#endif
C4JStorage::EMessageResult UIController::RequestUGCMessageBox(UINT, UINT, int, int (*)(LPVOID, int, const C4JStorage::EMessageResult), LPVOID) { return (C4JStorage::EMessageResult)0; }
C4JStorage::EMessageResult UIController::RequestContentRestrictedMessageBox(UINT, UINT, int, int (*)(LPVOID, int, const C4JStorage::EMessageResult), LPVOID) { return (C4JStorage::EMessageResult)0; }
void UIController::ShowUIDebugConsole(bool) {}
void UIController::ShowUIDebugMarketingGuide(bool) {}
void UIController::logDebugString(const string &) {}
UIScene *UIController::FindScene(EUIScene) { return NULL; }
void UIController::setFontCachingCalculationBuffer(int) {}

void UIController::preInit(S32, S32) {}
void UIController::postInit() {}
void UIController::renderScenes() {}

void ConsoleUIController::init(ID3D11Device *, ID3D11DeviceContext *, ID3D11RenderTargetView *, ID3D11DepthStencilView *, S32, S32) {}
void ConsoleUIController::render() {}
void ConsoleUIController::beginIggyCustomDraw4J(IggyCustomDrawCallbackRegion *, CustomDrawData *) {}
CustomDrawData *ConsoleUIController::setupCustomDraw(UIScene *, IggyCustomDrawCallbackRegion *) { return NULL; }
CustomDrawData *ConsoleUIController::calculateCustomDraw(IggyCustomDrawCallbackRegion *) { return NULL; }
void ConsoleUIController::endCustomDraw(IggyCustomDrawCallbackRegion *) {}
void ConsoleUIController::setTileOrigin(S32, S32) {}
GDrawTexture *ConsoleUIController::getSubstitutionTexture(int) { return NULL; }
void ConsoleUIController::destroySubstitutionTexture(void *, GDrawTexture *) {}
void ConsoleUIController::shutdown() {}

ConsoleUIController ui;

TileRenderer::TileRenderer() : setColor(true) {}
TileRenderer::TileRenderer(LevelSource *) : setColor(true) {}
TileRenderer::~TileRenderer() {}

Chunk::Chunk() {}
Chunk::~Chunk() {}

GuiComponent::GuiComponent() : blitOffset(0) {}
void GuiComponent::hLine(int, int, int, int) {}
void GuiComponent::vLine(int, int, int, int) {}
void GuiComponent::fill(int, int, int, int, int) {}
void GuiComponent::fillGradient(int, int, int, int, int, int) {}
void GuiComponent::drawCenteredString(Font *, const wstring &, int, int, int) {}
void GuiComponent::drawString(Font *, const wstring &, int, int, int) {}
void GuiComponent::blit(int, int, int, int, int, int) {}

Screen::Screen() {}
void Screen::render(int, int, float) {}
void Screen::keyPressed(wchar_t, int) {}
void Screen::mouseClicked(int, int, int) {}
void Screen::mouseReleased(int, int, int) {}
void Screen::buttonClicked(Button *) {}
void Screen::init(Minecraft *, int, int) {}
void Screen::setSize(int, int) {}
void Screen::init() {}
void Screen::updateEvents() {}
void Screen::mouseEvent() {}
void Screen::keyboardEvent() {}
void Screen::tick() {}
void Screen::removed() {}
void Screen::renderBackground() {}
void Screen::renderBackground(int) {}
void Screen::renderDirtBackground(int) {}
bool Screen::isPauseScreen() { return false; }
void Screen::confirmResult(bool, int) {}
void Screen::tabPressed() {}
wstring Screen::getClipboard() { return L""; }
void Screen::setClipboard(const wstring &) {}

Model::Model() : attackTime(0), riding(false), young(false), texWidth(64), texHeight(32), yHeadOffs(0), zHeadOffs(0) {}
OffsettedRenderList::OffsettedRenderList() : x(0), y(0), z(0), xOff(0), yOff(0), zOff(0), lists(NULL), inited(false), rendered(false) {}

void HumanoidModel::render(shared_ptr<Entity>, float, float, float, float, float, float, bool) {}
void HumanoidModel::setupAnim(float, float, float, float, float, float, unsigned int) {}
ModelPart *HumanoidModel::AddOrRetrievePart(SKIN_BOX *) { return NULL; }

void TitleScreen::tick() {}
void TitleScreen::keyPressed(wchar_t, int) {}
void TitleScreen::init() {}
void TitleScreen::buttonClicked(Button *) {}
void TitleScreen::render(int, int, float) {}

SoundEngine::SoundEngine() {}
void SoundEngine::SetStreamingSounds(int, int, int, int, int, int, int) {}
void SoundEngine::destroy() {}
void SoundEngine::play(int, float, float, float, float, float) {}
void SoundEngine::playStreaming(const wstring &, float, float, float, float, float, bool) {}
void SoundEngine::playUI(int, float, float) {}
void SoundEngine::playMusicTick() {}
void SoundEngine::updateMusicVolume(float) {}
void SoundEngine::updateSystemMusicPlaying(bool) {}
void SoundEngine::updateSoundEffectVolume(float) {}
void SoundEngine::init(Options *) {}
void SoundEngine::tick(shared_ptr<Mob> *, float) {}
void SoundEngine::add(const wstring &, File *) {}
void SoundEngine::addMusic(const wstring &, File *) {}
void SoundEngine::addStreaming(const wstring &, File *) {}
char *SoundEngine::ConvertSoundPathToName(const wstring &, bool) { return NULL; }

bool ConsoleSoundEngine::GetIsPlayingStreamingCDMusic() { return false; }
bool ConsoleSoundEngine::GetIsPlayingStreamingGameMusic() { return false; }
void ConsoleSoundEngine::SetIsPlayingStreamingCDMusic(bool) {}
void ConsoleSoundEngine::SetIsPlayingStreamingGameMusic(bool) {}
bool ConsoleSoundEngine::GetIsPlayingEndMusic() { return false; }
bool ConsoleSoundEngine::GetIsPlayingNetherMusic() { return false; }
void ConsoleSoundEngine::SetIsPlayingEndMusic(bool) {}
void ConsoleSoundEngine::SetIsPlayingNetherMusic(bool) {}

void LevelRenderer::staticCtor() {}
void IUIScene_CreativeMenu::staticCtor() {}
vector< shared_ptr<ItemInstance> > IUIScene_CreativeMenu::categoryGroups[IUIScene_CreativeMenu::eCreativeInventoryGroupsCount];
IUIScene_CreativeMenu::TabSpec **IUIScene_CreativeMenu::specs = NULL;

unsigned short SFontData::Codepoints[SFontData::FONTSIZE] = {};
SFontData SFontData::Mojangles_7 = {};
SFontData SFontData::Mojangles_11 = {};

int IUIScene_PauseMenu::SaveWorldThreadProc(void *) { return 0; }
int IUIScene_PauseMenu::ExitWorldThreadProc(void *) { return 0; }
void IUIScene_PauseMenu::_ExitWorld(LPVOID) {}
shared_ptr<Merchant> IUIScene_TradingMenu::getMerchant() { return shared_ptr<Merchant>(); }
bool IUIScene_CraftingMenu::isItemSelected(int) { return false; }

void UIScene_FullscreenProgress::SetWasCancelled(bool) {}

double Particle::xOff = 0;
double Particle::yOff = 0;
double Particle::zOff = 0;

void Particle::_init(Level *level, double x, double y, double z) {}
Particle::Particle(Level *level, double x, double y, double z) : Entity(level) {}
Particle::Particle(Level *level, double x, double y, double z, double xa, double ya, double za) : Entity(level) {}
shared_ptr<Particle> Particle::setPower(float) { return shared_ptr<Particle>(); }
shared_ptr<Particle> Particle::scale(float) { return shared_ptr<Particle>(); }
void Particle::setColor(float, float, float) {}
void Particle::setAlpha(float) {}
float Particle::getRedCol() { return 0; }
float Particle::getGreenCol() { return 0; }
float Particle::getBlueCol() { return 0; }
float Particle::getAlpha() { return 0; }
bool Particle::makeStepSound() { return false; }
void Particle::defineSynchedData() {}
void Particle::tick() {}
void Particle::render(Tesselator *, float, float, float, float, float, float) {}
int Particle::getParticleTexture() { return 0; }
void Particle::addAdditonalSaveData(CompoundTag *) {}
void Particle::readAdditionalSaveData(CompoundTag *) {}
void Particle::setTex(Textures *, Icon *) {}
void Particle::setMiscTex(int) {}
void Particle::setNextMiscAnimTex() {}
bool Particle::isAttackable() { return false; }
wstring Particle::toString() { return L""; }

void CritParticle::_init(Level *, shared_ptr<Entity>, ePARTICLE_TYPE) {}
CritParticle::CritParticle(Level *level, shared_ptr<Entity> entity) : Particle(level, 0, 0, 0, 0, 0, 0), life(0), lifeTime(0), particleName(eParticleType_crit) {}
CritParticle::CritParticle(Level *level, shared_ptr<Entity> entity, ePARTICLE_TYPE type) : Particle(level, 0, 0, 0, 0, 0, 0), life(0), lifeTime(0), particleName(type) {}
void CritParticle::CritParticlePostConstructor() {}
void CritParticle::render(Tesselator *, float, float, float, float, float, float) {}
void CritParticle::tick() {}
int CritParticle::getParticleTexture() { return 0; }

TakeAnimationParticle::TakeAnimationParticle(Level *level, shared_ptr<Entity> item, shared_ptr<Entity> target, float yOffs) : Particle(level, 0, 0, 0, 0, 0, 0), life(0), lifeTime(0), yOffs(yOffs) {}
TakeAnimationParticle::~TakeAnimationParticle() {}
void TakeAnimationParticle::render(Tesselator *, float, float, float, float, float, float) {}
void TakeAnimationParticle::tick() {}
int TakeAnimationParticle::getParticleTexture() { return 0; }

#ifdef _WIN32
void C4JStorage::SetMaxSaves(int) {}
C4JStorage::ESaveGameState C4JStorage::GetSaveState() { return C4JStorage::ESaveGame_Idle; }
#endif