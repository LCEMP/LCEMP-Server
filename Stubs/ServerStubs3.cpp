#include "stdafx.h"

#include "../../Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h"
#include "../../Minecraft.Client/Windows64/4JLibs/inc/4J_Storage.h"
#include "../../Minecraft.Client/Windows64/Windows64_App.h"
#include "../../Minecraft.World/Item.h"
#include "../../Minecraft.World/Tile.h"
#include "../../Minecraft.Client/DeathScreen.h"
#include "../../Minecraft.Client/ErrorScreen.h"
#include "../../Minecraft.Client/ChatScreen.h"
#include "../../Minecraft.Client/InBedChatScreen.h"
#include "../../Minecraft.Client/Common/UI/UIScene.h"

CConsoleMinecraftApp app;
C4JRender RenderManager;

CConsoleMinecraftApp::CConsoleMinecraftApp() : CMinecraftApp() {}
void CConsoleMinecraftApp::SetRichPresenceContext(int, int) {}
void CConsoleMinecraftApp::CaptureSaveThumbnail() {}
void CConsoleMinecraftApp::GetSaveThumbnail(PBYTE*, DWORD*) {}
void CConsoleMinecraftApp::FatalLoadError() {}
void CConsoleMinecraftApp::StoreLaunchData() {}
void CConsoleMinecraftApp::ExitGame() {}
void CConsoleMinecraftApp::ReleaseSaveThumbnail() {}
void CConsoleMinecraftApp::GetScreenshot(int, PBYTE*, DWORD*) {}
int  CConsoleMinecraftApp::LoadLocalTMSFile(WCHAR*) { return 0; }
int  CConsoleMinecraftApp::LoadLocalTMSFile(WCHAR*, eFileExtensionType) { return 0; }
void CConsoleMinecraftApp::FreeLocalTMSFiles(eTMSFileType) {}
int  CConsoleMinecraftApp::GetLocalTMSFileIndex(WCHAR*, bool, eFileExtensionType) { return -1; }
void CConsoleMinecraftApp::TemporaryCreateGameStart() {}

void C4JRender::Tick() {}
void C4JRender::UpdateGamma(unsigned short) {}
void C4JRender::StartFrame() {}
bool C4JRender::IsWidescreen() { return false; }
bool C4JRender::IsHiDef() { return false; }
void C4JRender::Present() {}
void C4JRender::CBuffLockStaticCreations() {}
void C4JRender::StateSetViewport(eViewportType) {}

const int Item::apple_Id;
const int Item::arrow_Id;
const int Item::beef_cooked_Id;
const int Item::beef_raw_Id;
const int Item::book_Id;
const int Item::boots_chain_Id;
const int Item::boots_cloth_Id;
const int Item::boots_diamond_Id;
const int Item::boots_iron_Id;
const int Item::bread_Id;
const int Item::chestplate_chain_Id;
const int Item::chestplate_cloth_Id;
const int Item::chestplate_diamond_Id;
const int Item::chestplate_iron_Id;
const int Item::chicken_cooked_Id;
const int Item::chicken_raw_Id;
const int Item::clock_Id;
const int Item::coal_Id;
const int Item::compass_Id;
const int Item::cookie_Id;
const int Item::diamond_Id;
const int Item::enderPearl_Id;
const int Item::expBottle_Id;
const int Item::eyeOfEnder_Id;
const int Item::fish_cooked_Id;
const int Item::flintAndSteel_Id;
const int Item::goldIngot_Id;
const int Item::hatchet_diamond_Id;
const int Item::hatchet_iron_Id;
const int Item::helmet_chain_Id;
const int Item::helmet_cloth_Id;
const int Item::helmet_diamond_Id;
const int Item::helmet_iron_Id;
const int Item::hoe_diamond_Id;
const int Item::hoe_iron_Id;
const int Item::ironIngot_Id;
const int Item::leggings_chain_Id;
const int Item::leggings_cloth_Id;
const int Item::leggings_diamond_Id;
const int Item::leggings_iron_Id;
const int Item::melon_Id;
const int Item::paper_Id;
const int Item::pickAxe_diamond_Id;
const int Item::pickAxe_iron_Id;
const int Item::porkChop_cooked_Id;
const int Item::porkChop_raw_Id;
const int Item::redStone_Id;
const int Item::rotten_flesh_Id;
const int Item::saddle_Id;
const int Item::seeds_melon_Id;
const int Item::seeds_pumpkin_Id;
const int Item::seeds_wheat_Id;
const int Item::shears_Id;
const int Item::shovel_diamond_Id;
const int Item::shovel_iron_Id;
const int Item::sword_diamond_Id;
const int Item::sword_iron_Id;
const int Item::wheat_Id;

const int Tile::bookshelf_Id;
const int Tile::cloth_Id;
const int Tile::glass_Id;
const int Tile::lightGem_Id;


void DeathScreen::init() {}
void DeathScreen::keyPressed(char, int) {}
void DeathScreen::buttonClicked(Button*) {}
void DeathScreen::render(int, int, float) {}
bool DeathScreen::isPauseScreen() { return false; }

ErrorScreen::ErrorScreen(const wstring&, const wstring&) {}
void ErrorScreen::init() {}
void ErrorScreen::render(int, int, float) {}
void ErrorScreen::keyPressed(wchar_t, int) {}

void InBedChatScreen::init() {}
void InBedChatScreen::removed() {}
void InBedChatScreen::keyPressed(wchar_t, int) {}
void InBedChatScreen::render(int, int, float) {}
void InBedChatScreen::buttonClicked(Button*) {}

UIScene::UIScene(int iPad, UILayer* parentLayer) : m_iPad(iPad), m_parentLayer(parentLayer), swf(nullptr), m_rootPath(nullptr), m_pItemRenderer(nullptr), m_iFocusControl(0), m_iFocusChild(0), m_lastOpacity(1.0f), m_bUpdateOpacity(false), m_bVisible(false), m_bCanHandleInput(false), m_backScene(nullptr), m_callbackUniqueId(0), m_loadedResolution((ESceneResolution)0), m_bIsReloading(false), m_bFocussedOnce(false), m_movieWidth(0), m_movieHeight(0), m_renderWidth(0), m_renderHeight(0), bHasFocus(false), m_hasTickedOnce(false), m_cacheSlotRenders(false), m_needsCacheRendered(false), m_expectedCachedSlotCount(0) {}
UIScene::~UIScene() {}
void UIScene::reloadMovie(bool) {}
bool UIScene::needsReloaded() { return false; }
bool UIScene::hasMovie() { return false; }
void UIScene::updateSafeZone() {}
F64 UIScene::getSafeZoneHalfHeight() { return 0; }
F64 UIScene::getSafeZoneHalfWidth() { return 0; }
bool UIScene::mapElementsAndNames() { return false; }
void UIScene::tick() {}
void UIScene::updateTooltips() {}
void UIScene::handleGainFocus(bool) {}
UIControl* UIScene::GetMainPanel() { return nullptr; }
void UIScene::render(S32, S32, C4JRender::eViewportType) {}
void UIScene::customDraw(IggyCustomDrawCallbackRegion*) {}
bool UIScene::allowRepeat(int) { return false; }
bool UIScene::isReadyToDelete() { return true; }
IggyName UIScene::registerFastName(const wstring&) { return 0; }

#include "../../Minecraft.Client/Common/UI/UIControl_Base.h"
UIControl_Base::UIControl_Base() {}
bool UIControl_Base::setupControl(UIScene*, IggyValuePath*, const string&) { return false; }
void UIControl_Base::tick() {}
void UIControl_Base::setLabel(const wstring&, bool, bool) {}
void UIControl_Base::setLabel(const string&) {}
void UIControl_Base::setAllPossibleLabels(int, wchar_t[][256]) {}
bool UIControl_Base::hasFocus() { return false; }

UIControl::UIControl() {}
bool UIControl::setupControl(UIScene*, IggyValuePath*, const string&) { return false; }
void UIControl::ReInit() {}
IggyValuePath* UIControl::getIggyValuePath() { return nullptr; }

#include "../../Minecraft.Client/Common/UI/UIControl_Label.h"
UIControl_Label::UIControl_Label() {}
bool UIControl_Label::setupControl(UIScene*, IggyValuePath*, const string&) { return false; }
void UIControl_Label::ReInit() {}

#include "../../Minecraft.Client/Common/UI/UIControl_SlotList.h"
UIControl_SlotList::UIControl_SlotList() {}
bool UIControl_SlotList::setupControl(UIScene*, IggyValuePath*, const string&) { return false; }
void UIControl_SlotList::setFocus(bool) {}

ChatScreen::ChatScreen() {}
void ChatScreen::init() {}
void ChatScreen::removed() {}
void ChatScreen::tick() {}
void ChatScreen::render(int, int, float) {}
void ChatScreen::keyPressed(wchar_t, int) {}
void ChatScreen::mouseClicked(int, int, int) {}

extern "C" {
IggyValuePath* IggyPlayerRootPath(Iggy*) { return nullptr; }
}

#include "../../Minecraft.Client/Common/UI/UIScene_TradingMenu.h"

UIScene_TradingMenu::UIScene_TradingMenu(int iPad, void* initData, UILayer* parentLayer) : UIScene(iPad, parentLayer), IUIScene_TradingMenu(), m_showingRightArrow(false), m_showingLeftArrow(false) {}
wstring UIScene_TradingMenu::getMoviePath() { return wstring(); }
void UIScene_TradingMenu::updateTooltips() {}
void UIScene_TradingMenu::handleDestroy() {}
void UIScene_TradingMenu::handleReload() {}
void UIScene_TradingMenu::tick() {}
void UIScene_TradingMenu::handleInput(int, int, bool, bool, bool, bool&) {}
void UIScene_TradingMenu::customDraw(IggyCustomDrawCallbackRegion*) {}
void UIScene_TradingMenu::showScrollRightArrow(bool) {}
void UIScene_TradingMenu::showScrollLeftArrow(bool) {}
void UIScene_TradingMenu::moveSelector(bool) {}
void UIScene_TradingMenu::setTitle(const wstring&) {}
void UIScene_TradingMenu::setRequest1Name(const wstring&) {}
void UIScene_TradingMenu::setRequest2Name(const wstring&) {}
void UIScene_TradingMenu::setRequest1RedBox(bool) {}
void UIScene_TradingMenu::setRequest2RedBox(bool) {}
void UIScene_TradingMenu::setTradeRedBox(int, bool) {}
void UIScene_TradingMenu::setOfferDescription(const wstring&, vector<wstring>&) {}

IUIScene_TradingMenu::IUIScene_TradingMenu() : m_menu(nullptr), m_validOffersCount(0), m_selectedSlot(0), m_offersStartIndex(0), m_bHasUpdatedOnce(false) {}
void IUIScene_TradingMenu::setRequest1Item(shared_ptr<ItemInstance>) {}
void IUIScene_TradingMenu::setRequest2Item(shared_ptr<ItemInstance>) {}
void IUIScene_TradingMenu::setTradeItem(int, shared_ptr<ItemInstance>) {}
