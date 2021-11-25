#include "pch.h"
#include "scene_logo.h"

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
	return Scene::NativeConstruct();
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
	return S_OK;
}
