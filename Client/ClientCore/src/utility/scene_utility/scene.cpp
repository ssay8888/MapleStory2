#include "pch.h"
#include "scene.h"

Scene::Scene(const ComPtr<IDirect3DDevice9>& device):
	_graphic_device(device)
{
}

auto Scene::NativeConstruct() -> HRESULT
{
	return S_OK;
}

auto Scene::Tick(double timeDelta) -> int32_t
{
	return int32_t();
}

auto Scene::Render() -> HRESULT
{
	return S_OK;
}
