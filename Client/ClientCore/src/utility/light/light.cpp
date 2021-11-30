#include "pch.h"
#include "light.h"

D3DLIGHT9 Light::GetLightDesc() const
{
	return _light_desc;
}

HRESULT Light::NativeConstruct(const D3DLIGHT9& lightDesc)
{
	_light_desc = lightDesc;
	return S_OK;
}

std::shared_ptr<Light> Light::Create(const D3DLIGHT9& lightDesc)
{
	auto pInstance = std::make_shared<Light>();

	if (FAILED(pInstance->NativeConstruct(lightDesc)))
	{
		MSGBOX("Failed to Creating Light");
		return nullptr;
	}
	return pInstance;
}
