#include "c_pch.h"
#include "character_select.h"

#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/ui/character_select/character_select_ui.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/camera/camera.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"

CharacterSelect::CharacterSelect(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

HRESULT CharacterSelect::NativeConstruct()
{

	BackGround::BackGroundInfo info;
	info.prototype_texture_name = TEXT("Prototype_Texture_GameSky");
	info.image_size_x = 1280;
	info.image_size_y = 720;
	info.pos_x = 1280 >> 1;
	info.pos_y = 720 >> 1;
	if (FAILED(ObjectManager::GetInstance().AddGameObject(kScene::kSceneCharacterSelect, TEXT("Prototype_BackGround"), TEXT("Layer_SKY"), &info)))
		return E_FAIL;

	if (FAILED(ReadyLight()))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
	//	return E_FAIL;

	if (FAILED(ReadyLayerSelectUi(TEXT("Layer_SelectUi"))))
		return E_FAIL; 


	const auto camera = ObjectManager::GetInstance().GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Camera"), 0);

	if (const auto transform = std::static_pointer_cast<Transform>(camera->GetComponentPtr(TEXT("Com_Transform"))))
	{
		transform->SetState(Transform::kState::kStateRight, _float3(-0.168381f, -0.f, -0.985721f));
		transform->SetState(Transform::kState::kStateUp, _float3(0.485701f, 0.870181f, -0.082967f));
		transform->SetState(Transform::kState::kStateLook, _float3(0.984797f, 0.043356f, -0.168223f));
		transform->SetState(Transform::kState::kStatePosition, _float3(-4.767985f, 4.256438f, 0.189017f));
	}
	return Scene::NativeConstruct();
}

int32_t CharacterSelect::Tick(double timeDelta)
{
	auto& instance = ObjectManager::GetInstance();

	auto selectUi = std::static_pointer_cast<CharacterSelectUi>(instance.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_SelectUi"), 0));
	switch(selectUi->GetState())
	{
	case CharacterSelectUi::kCharacterSelectState::kSelect: 
		break;
	case CharacterSelectUi::kCharacterSelectState::kCreateJob: 
		break;
	case CharacterSelectUi::kCharacterSelectState::kBeautyInit:
		if (FAILED(ReadyLayerPlayer(TEXT("Layer_Fittingdoll"))))
			return E_FAIL;
		selectUi->ChangeState(CharacterSelectUi::kCharacterSelectState::kBeauty);
		ReadyLayerBeautyUi(TEXT("Layer_Beauty"));
		break;
	case CharacterSelectUi::kCharacterSelectState::kBeauty:
		break;
	default: ;
	}
	return Scene::Tick(timeDelta);
}

HRESULT CharacterSelect::Render()
{
	return Scene::Render();
}

auto CharacterSelect::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CharacterSelect>
{
	auto pInstance = std::make_shared<CharacterSelect>(device);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CharacterSelect");
		return nullptr;
	}

	return pInstance;
}

auto CharacterSelect::ReadyLight() -> HRESULT
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

auto CharacterSelect::ReadyLayerCamera(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	Camera::CameraDesc			cameraDesc;
	cameraDesc.at = _float3(0.f, 0.f, 0.f);
	//cameraDesc.at = _float3(-972.4966f, 1088.36475f, 58.27349f ) / 150.f * 0.59f;
	//cameraDesc.at = _float3(-622.779358f, 1064.66284f, -16.07339f) / 150.f * 0.59f;

	//_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f, -16.07339f) / 150 * 0.59f);
	//_transform->SetState(Transform::kState::kStatePosition, _float3(885.f, 413.f, 1180.f));
	cameraDesc.eye = _float3(0.0f, 5.0f, -8.f);
	//cameraDesc.eye = _float3(-972.4966f, 1088.36475f + 500.f, 58.27349f - 800.f) / 150.f * 0.59f;
	cameraDesc.fovy = D3DXToRadian(25);
	cameraDesc.aspect = static_cast<float>(g_WinCX) / g_WinCY;
	cameraDesc.near_ = 0.25f;
	cameraDesc.far_ = 300;

	if (FAILED(objectManager.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Camera_Free"), pLayerTag, &cameraDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto CharacterSelect::ReadyLayerPlayer(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Mesh_Fittingdool"), pLayerTag, _float3(0.0f, 0, 0))))
	{
		return E_FAIL;
	}
	if (FAILED(objectManager.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Mesh_Weapon"), pLayerTag, _float3(0.0f, 0, 0))))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto CharacterSelect::ReadyLayerSelectUi(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Mesh_Character_Select_Ui"), pLayerTag)))
	{
		return E_FAIL;
	}


	return S_OK;
}

auto CharacterSelect::ReadyLayerBeautyUi(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Mesh_Character_Beauty_Ui"), pLayerTag)))
	{
		return E_FAIL;
	}
	return S_OK;
}
