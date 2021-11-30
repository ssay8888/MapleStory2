#include "pch.h"
#include "scene_logo.h"

#include <iostream>

#include "src/game_object/back_ground/back_ground.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/input/input_device.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

SceneLogo::SceneLogo(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

SceneLogo::~SceneLogo()
{
}

HRESULT SceneLogo::NativeConstruct()
{
	Scene::NativeConstruct();

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

int32_t SceneLogo::Tick(const double timeDelta)
{
	Scene::Tick(timeDelta);
	
	auto a = InputDevice::GetInstance().GetDirectKeyState(DIK_RETURN);
	if (a & 0x80)
	{
		const auto scene = SceneLoading::Create(_graphic_device, kScene::kSceneGamePlay0);
		if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
		{
			GameLogicManager::Clear(static_cast<uint32_t>(kScene::kSceneLogo));
		}
	}
	return S_OK;
}

HRESULT SceneLogo::Render()
{
	return Scene::Render();
}

auto SceneLogo::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<SceneLogo>
{
	auto instance = std::make_shared<SceneLogo>(device);

	if (FAILED(instance->NativeConstruct()))
	{
		MSGBOX("Failed to Create SceneLogo");
		return nullptr;
	}

	return instance;
}

auto SceneLogo::ReadyLayerBackGround(const std::wstring& layerTag) -> HRESULT
{
	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_BackGround"), layerTag)))
		return E_FAIL;

	TagTextBoxInformation info;
	info.pos = { 250, 250 };
	info.size = {300, 36};

	if (FAILED(ObjectManager::GetInstance().AddGameObject(static_cast<int32_t>(kScene::kSceneLogo), TEXT("Prototype_Login_Textbox"), layerTag, &info)))
		return E_FAIL;
	return S_OK;
}
