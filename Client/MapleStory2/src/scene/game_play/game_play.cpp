#include "c_pch.h"
#include "game_play.h"

#include "data_reader/data_reader_manager.h"
#include "protocol/game_protocol.pb.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/numbers/numbers.h"
#include "src/main/main_app.h"
#include "src/managers/sound_manager/sound_manager.h"
#include "src/network/game_server_packet_handler.h"
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
	{
		return E_FAIL;
	}



	if (FAILED(ReadyInventory()))
	{
		//return E_FAIL;
	}
	if (FAILED(ReadyLayerPlayer(TEXT("Layer_Character"))))
	{
	//	return E_FAIL;
	}

	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_PlayerInfo"), L"Layer_PlayerInfo")))
	{
		//return E_FAIL;
	}
	if (FAILED(ReadyLayerCamera(TEXT("Layer_Camera"))))
	{
	//	return E_FAIL;
	}

	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
	{
		//return E_FAIL;
	}

	if (FAILED(ReadyMonster()))
	{
		//return E_FAIL;
	}
	if (FAILED(ReadySkillUi()))
	{
		//return E_FAIL;
	}

	if (FAILED(ReadyKeySetManager()))
	{
	//	return E_FAIL;
	}

	if (FAILED(ReadyChatUi()))
	{
		//return E_FAIL;
	}
	Protocol::GameClientLoading sendPkt;
	auto authInfo = g_mainApp->GetAuthInfo();
	sendPkt.set_auth(authInfo.auth());
	sendPkt.set_state(Protocol::kLoadSuccess);
	g_service->Broadcast(GameServerPacketHandler::MakeSendBuffer(sendPkt));
	std::this_thread::sleep_for(100ms);
	SoundManager::GetInstance().StopSound(SoundManager::kBgm);
	SoundManager::GetInstance().PlayBGM(L"BGM_Henesys_01.wav");
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
	auto object = objectManager.GetGameObjectPtr(kScene::kSceneStatic, pLayerTag, 0);
	if (object == nullptr)
	{
		if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Player"), pLayerTag)))
		{
			//return E_FAIL;
		}
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
	/*const auto monsters = DataReaderManager::GetInstance().AllMonsterInfo();
	for (auto monster : monsters)
	{
		wchar_t LayerTag[MAX_PATH];
		swprintf_s(LayerTag, L"Layer_Monster_%d", monster->id);
		if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Mesh_Monster"), LayerTag, &monster)))
		{
			return E_FAIL;
		}
	}*/

	Numbers::NumberInfo info;
	info.is_player_attack = true;
	info.numbers = 1234567890;
	info.parent_transform = nullptr;
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Numbers"), L"Layer_Numbers", &info)))
	{
		return E_FAIL;
	}


	return S_OK;
}

auto GamePlay::ReadyInventory() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Inventory"), L"Layer_Inventory")))
	{
		//return E_FAIL;
	}
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_EquippedUi"), L"Layer_EquippedUi")))
	{
		//return E_FAIL;
	}

	return S_OK;
}

auto GamePlay::ReadyKeySetManager() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_KeySetManager"), L"Layer_Keymanager")))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

auto GamePlay::ReadySkillUi() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_SkillUi"), L"Layer_SkillUi")))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto GamePlay::ReadyChatUi() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_ChatUi"), L"Layer_ChatUi")))
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
