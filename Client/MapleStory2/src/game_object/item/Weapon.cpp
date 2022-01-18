#include "c_pch.h"
#include "Weapon.h"

#include "data_reader/data_reader_manager.h"
#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/game_object/player/player.h"
#include "src/game_object/user/user.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Weapon::Weapon(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

Weapon::~Weapon()
{
}

HRESULT Weapon::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Weapon::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (arg == nullptr)
	{
		return E_FAIL;
	}

	_weapon_info = *static_cast<WeaponInfo*>(arg);

	if (FAILED(AddComponents()))
		return E_FAIL;

	//D3DXMatrixIdentity(&_origin_matrix);
	D3DXMatrixRotationX(&_origin_matrix, D3DXToRadian(90.f));
	const auto& instance = ObjectManager::GetInstance();

	std::wstring layerName;

	if (_weapon_info.player_id == GameLogicQueue::GetInstance()->GetCharacterInfo().character_id())
	{
		layerName.append(L"Layer_Character");
	}
	else
	{
		layerName.append(L"Layer_User_").append(std::to_wstring(_weapon_info.player_id));
	}
	_parent_world_matrix = &_weapon_info.transform->GetWorldMatrix();

	//const auto playerMesh = std::static_pointer_cast<MeshDynamic>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Player"), TEXT("Com_Mesh_4"), 0));
	//_parent_bone_matrix = playerMesh->GetBoneMatrixPointer("Weapon_Back_R_Point");
	return S_OK;
}

auto Weapon::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto Weapon::LateTick(const double timeDelta) -> HRESULT
{
	GameObject::LateTick(timeDelta);

	auto& objectManager = ObjectManager::GetInstance();

	std::wstring layerName;

	if (_weapon_info.player_id == GameLogicQueue::GetInstance()->GetCharacterInfo().character_id())
	{
		layerName.append(L"Layer_Character");
		auto playerObject = std::static_pointer_cast<Player>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layerName, 0));
		_player_current_mesh = playerObject->GetCurrentDynamicMesh();


	}
	else
	{
		layerName.append(L"Layer_User_").append(std::to_wstring(_weapon_info.player_id));
		auto playerObject = std::static_pointer_cast<User>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layerName, 0));
		_player_current_mesh = playerObject->GetCurrentDynamicMesh();

	}
	_parent_bone_matrix = _player_current_mesh.first->GetBoneMatrixPointer(_player_current_mesh.second->GetAnimationInfo()->target_node.c_str());

	_matrix		WorldMatrix = _origin_matrix * (*_parent_bone_matrix * *_parent_world_matrix);
	_transform_com->SetState(Transform::kState::kStateRight, *reinterpret_cast<_float3*>(&WorldMatrix.m[0][0]));
	_transform_com->SetState(Transform::kState::kStateUp, *reinterpret_cast<_float3*>(&WorldMatrix.m[1][0]));
	_transform_com->SetState(Transform::kState::kStateLook, *reinterpret_cast<_float3*>(&WorldMatrix.m[2][0]));
	_transform_com->SetState(Transform::kState::kStatePosition, *reinterpret_cast<_float3*>(&WorldMatrix.m[3][0]));
	
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return S_OK;
}

HRESULT Weapon::Render()
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);


	const uint32_t	numMaterials = _mesh_com->GetNumMaterials();

	for (uint32_t i = 0; i < numMaterials; ++i)
	{
		if (FAILED(_mesh_com->SetUpTextureOnShader(_shader_com, "g_DiffuseTexture", MeshMaterialTexture::kType::kTypeDiffuse, i)))
			return E_FAIL;
		
		_shader_com->Commit();

		if (FAILED(_mesh_com->Render(i)))
			return E_FAIL;
	}

	result = _shader_com->EndShader();
	return S_OK;
}

auto Weapon::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	std::shared_ptr<Weapon> pInstance = std::make_shared<Weapon>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Weapon");
	}
	return pInstance;
}

auto Weapon::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Weapon>
{
	std::shared_ptr<Weapon> pInstance = std::make_shared<Weapon>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Weapon");
	}
	return pInstance;
}

HRESULT Weapon::AddComponents()
{

	Transform::TransformDesc		TransformDesc;
	TransformDesc.speed_per_sec = 5.0f;
	TransformDesc.rotation_per_sec = D3DXToRadian(90.f);


	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &TransformDesc)))
		return E_FAIL;

	std::wstring prototypeName(L"Prototype_ItemModel_");
	prototypeName.append(std::to_wstring(_weapon_info.item_id)).append(L"_0");
	if (FAILED(AddComponent(kSceneStatic, prototypeName, TEXT("Com_Weapon"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
		return E_FAIL;
	
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}

HRESULT Weapon::SetUpConstantTable()
{
	auto& pipeline = PipeLine::GetInstance();
	const auto view = pipeline.GetTransform(D3DTS_VIEW);
	const auto project = pipeline.GetTransform(D3DTS_PROJECTION);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", _transform_com->GetWorldMatrix(), sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &project, sizeof(_matrix));

	const D3DLIGHT9	lightDesc = LightManager::GetInstance().GetLightDesc(0);
	const auto lightDir = _float4(lightDesc.Direction, 0.f);
	result = _shader_com->SetUpConstantTable("g_vLightDir", &lightDir, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightDiffuse", &lightDesc.Diffuse, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightAmbient", &lightDesc.Ambient, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightSpecular", &lightDesc.Specular, sizeof(_float4));
	const auto camPos = _float4(pipeline.GetCamPosition(), 1.f);
	result = _shader_com->SetUpConstantTable("g_vCamPosition", &camPos, sizeof(_float4));
	return S_OK;
}
