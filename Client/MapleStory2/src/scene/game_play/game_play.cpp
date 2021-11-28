#include "pch.h"
#include "game_play.h"

#include "src/utility/game_objects/camera/camera.h"
#include "src/utility/game_objects/manager/object_manager.h"

GamePlay::GamePlay(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

HRESULT GamePlay::NativeConstruct()
{
	Scene::NativeConstruct();

	if (FAILED(ReadyLight()))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;

}

auto GamePlay::Tick(const double timeDelta)->int32_t
{
	return Scene::Tick(timeDelta);
}

auto GamePlay::Render()->HRESULT
{
	return Scene::Render();
}

auto GamePlay::ReadyLight() -> HRESULT
{
	/* 방향성 광원을 셋팅한다. */
	D3DLIGHT9			LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Direction = _float3(1.f, -1.f, 1.f);
	_graphic_device->SetLight(0, &LightDesc);
	_graphic_device->LightEnable(0, TRUE);

	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));
	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.0f, 0.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(1.f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Position = _float3(5.0f, 3.f, 5.f);
	LightDesc.Range = 15.f;
	LightDesc.Attenuation1 = 1.f;
	_graphic_device->SetLight(1, &LightDesc);
	_graphic_device->LightEnable(1, TRUE);
	return S_OK;
}

auto GamePlay::ReadyLayerCamera(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	Camera::CameraDesc			cameraDesc;
	cameraDesc.at = _float3(0.f, 0.f, 0.f);
	cameraDesc.eye = _float3(0.0f, 5.0f, -8.f);
	cameraDesc.fovy = D3DXToRadian(60.0f);
	cameraDesc.aspect = static_cast<float>(g_WinCX) / g_WinCY;
	cameraDesc.near_ = 0.25f;
	cameraDesc.far_ = 300.0f;

	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Camera_Free"), pLayerTag, &cameraDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto GamePlay::ReadyLayerPlayer(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Player"), pLayerTag)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

auto GamePlay::ReadyLayerBackGround(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Terrain"), pLayerTag)))
	{
		return E_FAIL;
	}

	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Sky"), pLayerTag)))
	{
		return E_FAIL;
	}

	return S_OK;
}

std::shared_ptr<GamePlay> GamePlay::Create(const ComPtr<IDirect3DDevice9>& device)
{
	auto pInstance = std::make_shared<GamePlay>(device);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating LevelGamePlay");
		return nullptr;
	}

	return pInstance;
}
