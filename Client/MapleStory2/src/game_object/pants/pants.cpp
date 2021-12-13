#include "c_pch.h"
#include "pants.h"

#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/dynamic/animation/animation.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Pants::Pants(const ComPtr<IDirect3DDevice9>& device) :
	GameObject(device)
{
}

Pants::~Pants()
{
}

HRESULT Pants::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Pants::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);

	_base = ((*(_float3*)arg).x == 0);
	if (FAILED(AddComponents()))
		return E_FAIL;

	//D3DXMatrixIdentity(&_origin_matrix);
	D3DXMatrixRotationX(&_origin_matrix, D3DXToRadian(90.f));
	const auto& instance = ObjectManager::GetInstance();
	auto playerTransform = std::static_pointer_cast<Transform>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), TEXT("Com_Transform"), 0));
	_parent_world_matrix = &playerTransform->GetWorldMatrix();

	_transform_com->SetUpRotation(_float3(0, -1, 0), D3DXToRadian(255.924316f));
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f - 40.f, -16.07339f) / 150 * 0.58f);
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	return S_OK;
}

int32_t Pants::Tick(double timeDelta)
{
	const auto& instance = ObjectManager::GetInstance();
	auto playerObject = std::static_pointer_cast<Fittingdoll>(instance.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
	_player_current_mesh = playerObject->GetCurrentDynamicMesh();

	_parent_bone_matrix = _player_current_mesh.first->GetBoneMatrixPointer("PA_Panty");

	_matrix		WorldMatrix = *_parent_bone_matrix * *_parent_world_matrix;

	//_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f + x, 1064.66284f - y, -16.07339f + z) / 150 * 0.58f);
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2] + 0.5f));
	return GameObject::Tick(timeDelta);
}

int32_t Pants::LateTick(double timeDelta)
{
	GameObject::LateTick(timeDelta);
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return S_OK;
}

HRESULT Pants::Render()
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);

	auto mesh = _mesh_com;

	size_t iNumMeshContainers = mesh->GetNumMeshContainer();
	const auto& instance = ObjectManager::GetInstance();
	const auto player = std::static_pointer_cast<Fittingdoll>(instance.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));

	auto playerMesh = player->GetCurrentDynamicMesh();

	auto rootFrame = playerMesh.first->GetRootFrame();
	mesh->TargerCombinedTransformationMatrices(mesh->GetRootFrame(), rootFrame);
	for (uint32_t i = 0; i < iNumMeshContainers; ++i)
	{
		const uint32_t iNumMaterials = mesh->GetNumMaterials(i);

		mesh->UpdateSkinnedMesh(i);

		for (uint32_t j = 0; j < iNumMaterials; ++j)
		{
			if (FAILED(mesh->SetUpTextureOnShader(_shader_com, "g_DiffuseTexture", MeshMaterialTexture::kType::kTypeDiffuse, i, j)))
				return E_FAIL;

			_shader_com->Commit();

			mesh->Render(i, j);
		}
	}

	result = _shader_com->EndShader();
	return S_OK;
}

auto Pants::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	std::shared_ptr<Pants> pInstance = std::make_shared<Pants>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Item");
	}
	return pInstance;
}

auto Pants::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Pants>
{
	std::shared_ptr<Pants> pInstance = std::make_shared<Pants>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Item");
	}
	return pInstance;
}

auto Pants::GetMesh() -> std::shared_ptr<MeshDynamic>
{
	return _mesh_com;
}

HRESULT Pants::AddComponents()
{

	Transform::TransformDesc		TransformDesc;
	TransformDesc.speed_per_sec = 5.0f;
	TransformDesc.rotation_per_sec = D3DXToRadian(90.f);


	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &TransformDesc)))
		return E_FAIL;
	if (_base)
	{
		if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Mesh_Pants_Base"), TEXT("Com_Mesh_0"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Mesh_Pants"), TEXT("Com_Mesh_0"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
			return E_FAIL;

	}
	if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}

HRESULT Pants::SetUpConstantTable()
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
