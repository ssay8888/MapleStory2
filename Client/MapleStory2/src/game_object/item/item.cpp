#include "c_pch.h"
#include "item.h"

#include "data_reader/data_reader_manager.h"
#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Item::Item(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

Item::~Item()
{
}

HRESULT Item::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Item::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);

	if (FAILED(AddComponents()))
		return E_FAIL;

	//D3DXMatrixIdentity(&_origin_matrix);
	D3DXMatrixRotationX(&_origin_matrix, D3DXToRadian(90.f));
	const auto& instance = ObjectManager::GetInstance();
	auto playerTransform = std::static_pointer_cast<Transform>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), TEXT("Com_Transform"), 0));
	_parent_world_matrix = &playerTransform->GetWorldMatrix();

	//const auto playerMesh = std::static_pointer_cast<MeshDynamic>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Player"), TEXT("Com_Mesh_4"), 0));
	//_parent_bone_matrix = playerMesh->GetBoneMatrixPointer("Weapon_Back_R_Point");
	return S_OK;
}

int32_t Item::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t Item::LateTick(const double timeDelta)
{
	GameObject::LateTick(timeDelta);

	auto& objectManager = ObjectManager::GetInstance();
	auto playerObject = std::static_pointer_cast<Fittingdoll>(objectManager.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
	_player_current_mesh = playerObject->GetCurrentDynamicMesh();

	_parent_bone_matrix = _player_current_mesh.first->GetBoneMatrixPointer(_player_current_mesh.second->GetAnimationInfo()->target_node.c_str());

	_matrix		WorldMatrix = _origin_matrix * (*_parent_bone_matrix * *_parent_world_matrix);
	_transform_com->SetState(Transform::kState::kStateRight, *reinterpret_cast<_float3*>(&WorldMatrix.m[0][0]));
	_transform_com->SetState(Transform::kState::kStateUp, *reinterpret_cast<_float3*>(&WorldMatrix.m[1][0]));
	_transform_com->SetState(Transform::kState::kStateLook, *reinterpret_cast<_float3*>(&WorldMatrix.m[2][0]));
	_transform_com->SetState(Transform::kState::kStatePosition, *reinterpret_cast<_float3*>(&WorldMatrix.m[3][0]));

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return S_OK;
}

HRESULT Item::Render()
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

auto Item::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	std::shared_ptr<Item> pInstance = std::make_shared<Item>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Item");
	}
	return pInstance;
}

auto Item::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Item>
{
	std::shared_ptr<Item> pInstance = std::make_shared<Item>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Item");
	}
	return pInstance;
}

HRESULT Item::AddComponents()
{

	Transform::TransformDesc		TransformDesc;
	TransformDesc.speed_per_sec = 5.0f;
	TransformDesc.rotation_per_sec = D3DXToRadian(90.f);


	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &TransformDesc)))
		return E_FAIL;

	if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Mesh_Weapon_2"), TEXT("Com_Mesh_0"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
		return E_FAIL;
	
	if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}

HRESULT Item::SetUpConstantTable()
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
