#include "pch.h"
#include "light_manager.h"

#include "light.h"

auto LightManager::GetLightDesc(const int32_t index) -> D3DLIGHT9
{
	if (_lights.size() <= index)
	{
		return D3DLIGHT9();
	}
	return _lights[index]->GetLightDesc();
}

auto LightManager::AddLight(const D3DLIGHT9 lightDesc) -> HRESULT
{
	const auto light = Light::Create(lightDesc);
	if (nullptr == light)
		return E_FAIL;
	  
	_lights.push_back(light);
	return S_OK;
}
