#include "c_pch.h"
#include "hair.h"

#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/dynamic/animation/animation.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Hair::Hair(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

Hair::~Hair()
{
}

HRESULT Hair::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Hair::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);

	if (FAILED(AddComponents()))
		return E_FAIL;

	//D3DXMatrixIdentity(&_origin_matrix);
	D3DXMatrixRotationX(&_origin_matrix, D3DXToRadian(90.f));
	const auto& instance = ObjectManager::GetInstance();
	auto playerTransform = std::static_pointer_cast<Transform>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), TEXT("Com_Transform"), 0));
	_parent_world_matrix = &playerTransform->GetWorldMatrix();

	//_transform_com->SetState(Transform::kState::kStatePosition, _info.pos);
	_transform_com->SetUpRotation(_float3(0, -1, 0), D3DXToRadian(255.924316f));
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f - 40.f, -16.07339f) / 150 * 0.58f);
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	const auto player = std::static_pointer_cast<Fittingdoll>(instance.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
	//if (_meshs[0] != mesh)
	{
		LPD3DXANIMATIONSET		pAS = nullptr;
		auto mesh = player->GetCurrentDynamicMesh();
		mesh.first->GetAnimation()->GetAnimationController()->GetAnimationSet(0, &pAS);
		if (FAILED(_mesh_com->GetAnimation()->GetAnimationController()->RegisterAnimationSet(pAS)))
		{
			std::cout << "애니메이션 추가도중 실패함" << std::endl;
		}
		pAS->Release();
		mesh.first->GetAnimation()->GetAnimationController()->GetAnimationSet(1, &pAS);
		if (FAILED(_mesh_com->GetAnimation()->GetAnimationController()->RegisterAnimationSet(pAS)))
		{
			std::cout << "애니메이션 추가도중 실패함" << std::endl;
		}
		pAS->Release();
	}
	_mesh_com->GetAnimation()->SetAnimationIndex(2);
	//const auto playerMesh = std::static_pointer_cast<MeshDynamic>(instance.GetComponentPtr(kSceneCharacterSelect, TEXT("Layer_Player"), TEXT("Com_Mesh_4"), 0));
	//_parent_bone_matrix = playerMesh->GetBoneMatrixPointer("Weapon_Back_R_Point");
	return S_OK;
}

int32_t Hair::Tick(double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t Hair::LateTick(double timeDelta)
{
	GameObject::LateTick(timeDelta);

	/*auto& objectManager = ObjectManager::GetInstance();
	auto playerObject = std::static_pointer_cast<Fittingdoll>(objectManager.GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
	_player_current_mesh = playerObject->GetCurrentDynamicMesh();
	
	_parent_bone_matrix = _player_current_mesh.first->GetBoneMatrixPointer("Bip01_Spine");
	
	_matrix		WorldMatrix = _origin_matrix  * (*_parent_bone_matrix * *_parent_world_matrix);
	_transform_com->SetState(Transform::kState::kStateRight, *reinterpret_cast<_float3*>(&WorldMatrix.m[0][0]));
	_transform_com->SetState(Transform::kState::kStateUp, *reinterpret_cast<_float3*>(&WorldMatrix.m[1][0]));
	_transform_com->SetState(Transform::kState::kStateLook, *reinterpret_cast<_float3*>(&WorldMatrix.m[2][0]));
	_transform_com->SetState(Transform::kState::kStatePosition, *reinterpret_cast<_float3*>(&WorldMatrix.m[3][0]));*/

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	auto mesh = _mesh_com;
	mesh->PlayAnimation(timeDelta);
	return GameObject::LateTick(timeDelta);
}

HRESULT Hair::Render()
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);

	auto mesh = _mesh_com;

	size_t iNumMeshContainers = mesh->GetNumMeshContainer();

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

auto Hair::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	std::shared_ptr<Hair> pInstance = std::make_shared<Hair>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Item");
	}
	return pInstance;
}

auto Hair::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Hair>
{
	std::shared_ptr<Hair> pInstance = std::make_shared<Hair>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Item");
	}
	return pInstance;
}

HRESULT Hair::AddComponents()
{

	Transform::TransformDesc		TransformDesc;
	TransformDesc.speed_per_sec = 5.0f;
	TransformDesc.rotation_per_sec = D3DXToRadian(90.f);


	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &TransformDesc)))
		return E_FAIL;

	if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Mesh_Hair"), TEXT("Com_Mesh_0"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
		return E_FAIL;

	if (FAILED(AddComponent(kSceneCharacterSelect, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}

HRESULT Hair::SetUpConstantTable()
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
