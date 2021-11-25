#include "pch.h"
#include "main_app.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/picking/picking.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"

MainApp::MainApp()
{
	NativeConstruct();
}

auto MainApp::NativeConstruct() -> HRESULT
{
	GameLogicManager::InitDevice(g_hInst, g_Wnd, static_cast<int32_t>(kScene::kSceneEnd));


	if (FAILED(GraphicDevice::GetInstance().ReadyGraphicDevice(g_Wnd, GraphicDevice::kWindowMode::kModeWin, g_WinCX, g_WinCY, &_graphic_device)))
		return E_FAIL;

	if (FAILED(AddPrototypeComponent()))
		return E_FAIL;

	if (FAILED(AddPrototypeGameObject()))
		return E_FAIL;
	return S_OK;
}

auto MainApp::Tick(const double timeDelta) -> int32_t
{
	GameLogicManager::Tick(timeDelta);

	Picking::GetInstance().ComputeMouseCursorPosInWorld(g_Wnd);

	GameLogicManager::LateTick(timeDelta);
	return int32_t();
}

auto MainApp::RenderMainApp() -> HRESULT
{
	return S_OK;
}

HRESULT MainApp::AddPrototypeGameObject()
{
	return S_OK;
}

HRESULT MainApp::AddPrototypeComponent()
{
	return S_OK;
}
