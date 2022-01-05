#include "c_pch.h"
#include "game_play.h"

#include "data_reader/data_reader_manager.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/utility/game_objects/camera/camera.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"

GamePlay::GamePlay(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

HRESULT GamePlay::NativeConstruct()
{
	Scene::NativeConstruct();

	BackGround::BackGroundInfo info;
	info.prototype_texture_name = TEXT("Prototype_Texture_GameSky_Henesys");
	info.image_size_x = 1280;
	info.image_size_y = 720;
	info.pos_x = 1280 >> 1;
	info.pos_y = 720 >> 1;
	if (FAILED(ObjectManager::GetInstance().AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_BackGround"), TEXT("Layer_SKY"), &info)))
		return E_FAIL;

	if (FAILED(ReadyLight()))
		return E_FAIL;

	if (FAILED(ReadyLayerPlayer(TEXT("Layer_Character"))))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(ReadyMonster()))
		return E_FAIL;

	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_PlayerInfo"), L"Layer_PlayerInfo")))
	{
		return E_FAIL;
	}
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
	/* ���⼺ ������ �����Ѵ�. */
	D3DLIGHT9			lightDesc;
	ZeroMemory(&lightDesc, sizeof(D3DLIGHT9));
	lightDesc.Type = D3DLIGHT_DIRECTIONAL;
	lightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	lightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	lightDesc.Direction = _float3(1.f, -1.f, 1.f);

	LightManager::GetInstance().AddLight(lightDesc);

	return S_OK;
}

auto GamePlay::ReadyLayerCamera(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	Camera::CameraDesc			cameraDesc;
	cameraDesc.at = _float3(0.f, 0.f, 0.f);

	//_transform->SetState(Transform::kState::kStatePosition, _float3(885.f, 413.f, 1180.f));
	cameraDesc.eye = _float3(0.0f, 5.0f, -8.f);
	cameraDesc.fovy = D3DXToRadian(90.0f);
	cameraDesc.aspect = static_cast<float>(g_WinCX) / g_WinCY;
	cameraDesc.near_ = 0.2f;
	cameraDesc.far_ = 100.0f;

	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Ingame_Camera"), pLayerTag, &cameraDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto GamePlay::ReadyLayerPlayer(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Player"), pLayerTag)))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto GamePlay::ReadyLayerBackGround(const std::wstring& pLayerTag) -> HRESULT
{/*
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Terrain"), pLayerTag)))
	{
		return E_FAIL;
	}

	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Sky"), pLayerTag)))
	{
		return E_FAIL;
	}*/
	return S_OK;
}

auto GamePlay::ReadyMonster() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	const auto monsters = DataReaderManager::GetInstance().AllMonsterInfo();
	for (auto monster : monsters)
	{
		wchar_t LayerTag[MAX_PATH];
		swprintf_s(LayerTag, L"Layer_Monster_%d", monster->id);
		if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Mesh_Monster"), LayerTag, &monster)))
		{
			return E_FAIL;
		}
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
