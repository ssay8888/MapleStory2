#include "pch.h"
#include "main_app.h"

#include "client_defines.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/camera/camera_free.h"
#include "src/scene/logo/scene_logo.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/picking/picking.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

MainApp::MainApp()
{
	NativeConstruct();
}

auto MainApp::NativeConstruct() -> HRESULT
{
	GameLogicManager::InitDevice(g_hInst, g_Wnd, static_cast<int32_t>(kScene::kSceneEnd));


	if (FAILED(GraphicDevice::GetInstance().ReadyGraphicDevice(g_Wnd, GraphicDevice::kWindowMode::kModeWin, g_WinCX, g_WinCY, &_graphic_device)))
		return E_FAIL;

	GraphicDevice::GetInstance().GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (FAILED(AddPrototypeComponent()))
		return E_FAIL;

	if (FAILED(AddPrototypeGameObject()))
		return E_FAIL;

	SceneManager::GetInstance().SetUpScene(SceneLogo::Create(_graphic_device));
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
	auto& device = GraphicDevice::GetInstance();
	auto& renderer = Renderer::GetInstance();
	const auto& sceneManager = SceneManager::GetInstance();
	device.RenderBegin();

	renderer.RenderGameObject();

	if (FAILED(sceneManager.Render()))
		return E_FAIL;

	device.RenderEnd();
	return S_OK;
}

HRESULT MainApp::AddPrototypeGameObject()
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_BackGround"), BackGround::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Camera_Free"), CameraFree::Create(_graphic_device))))
		return E_FAIL;

	return S_OK;
}

HRESULT MainApp::AddPrototypeComponent()
{
	const auto& componentManager = ComponentManager::GetInstance();

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Transform"), Transform::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_VIBuffer_Rect"), ViBufferRect::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Shader_Rect"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/shader_rect.hlsl")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Texture_Default"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Default.jpg")))))
		return E_FAIL;


	return S_OK;
}
