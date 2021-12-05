#include "c_pch.h"
#include "character_select.h"

#include "src/utility/game_objects/camera/camera.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"

CharacterSelect::CharacterSelect(const ComPtr<IDirect3DDevice9>& device):
	Scene(device)
{
}

HRESULT CharacterSelect::NativeConstruct()
{

	if (FAILED(ReadyLight()))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
		return E_FAIL;

	return Scene::NativeConstruct();
}

int32_t CharacterSelect::Tick(double timeDelta)
{
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
	//cameraDesc.at = _float3(0.f, 0.f, 0.f);
	//cameraDesc.at = _float3(-972.4966f, 1088.36475f, 58.27349f ) / 150.f * 0.59f;
	cameraDesc.at = _float3(-622.779358f, 1064.66284f, -16.07339f) / 150.f * 0.59f;

	//_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f, -16.07339f) / 150 * 0.59f);
	//_transform->SetState(Transform::kState::kStatePosition, _float3(885.f, 413.f, 1180.f));
	//cameraDesc.eye = _float3(0.0f, 5.0f, -8.f);
	cameraDesc.eye = _float3(-972.4966f, 1088.36475f, 58.27349f) / 150.f * 0.59f;
	cameraDesc.fovy = D3DXToRadian(90.f);
	cameraDesc.aspect = static_cast<float>(g_WinCX) / g_WinCY;
	cameraDesc.near_ = 0.2f;
	cameraDesc.far_ = 10000.0f;

	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneCharacterSelect), TEXT("Prototype_Camera_Free"), pLayerTag, &cameraDesc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto CharacterSelect::ReadyLayerPlayer(const std::wstring& pLayerTag) -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddGameObject(static_cast<int32_t>(kScene::kSceneCharacterSelect), TEXT("Prototype_Mesh_Fittingdool"), pLayerTag, _float3(0.0f, 0, 0))))
	{
		return E_FAIL;
	}

	return S_OK;
}
