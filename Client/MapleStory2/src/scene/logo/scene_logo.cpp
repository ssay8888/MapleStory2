#include "pch.h"
#include "scene_logo.h"

#include "client_defines.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/utility/game_objects/manager/object_manager.h"

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

int32_t SceneLogo::Tick(double timeDelta)
{
	return Scene::Tick(timeDelta);
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

	return S_OK;
}
