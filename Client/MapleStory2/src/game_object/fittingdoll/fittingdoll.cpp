#include "c_pch.h"
#include "fittingdoll.h"

#include "data_reader/data_reader_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/dynamic/animation/animation.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"
#include "src/utils/file_utils.h"
#include "string_utils/string_utils.h"

Fittingdoll::Fittingdoll(const ComPtr<IDirect3DDevice9>& device) :
	GameObject(device)
{
}

HRESULT Fittingdoll::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Fittingdoll::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (arg)
	{
		_info = *static_cast<FittingdollInfo*>(arg);
	}
	if (FAILED(AddComponents()))
		return E_FAIL;
	_transform_com->SetState(Transform::kState::kStatePosition, _info.pos);
	_transform_com->SetUpRotation(_float3(0, -1, 0), D3DXToRadian(255.924316f));
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f-40.f, -16.07339f) / 150 * 0.58f);
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	_meshs[0]->SetAnimationIndex(1);
	_current_mesh_num = 1;
	//_transform_com->SetUpRotation(_float3(1,  1, 0), D3DXToRadian(90));
	return S_OK;
}

int32_t Fittingdoll::Tick(double timeDelta)
{
	//static int32_t radian = 0;
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_Z))
	//{
	//	_transform_com->SetUpRotation(_float3(1, 0, 0), D3DXToRadian(radian++));
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_X))
	//{
	//	_transform_com->SetUpRotation(_float3(0, 1, 0), D3DXToRadian(radian++));
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_C))
	//{
	//	_transform_com->SetUpRotation(_float3(0, 0, 1), D3DXToRadian(radian++));
	//}
	//_mesh2_com->SetAnimationIndex(0, _mesh_com->GetAnimation()->GetAnimationController());


	if (InputDevice::GetInstance().GetKeyDown(DIK_X))
	{
		_is_idle = false;
		_new_mesh_num = (_current_mesh_num + 1) % 4;
		_meshs[0]->SetAnimationIndex(_new_mesh_num);
		_current_mesh_num = _new_mesh_num;
	}

	if (InputDevice::GetInstance().GetKeyPressing(DIK_C))
	{
		_is_idle = true;
		_new_mesh_num = 3;
		_meshs[0]->SetAnimationIndex(_new_mesh_num);
		_meshs[_current_mesh_num]->ResetAnimation();
		_current_mesh_num = 3;
	}

	if (InputDevice::GetInstance().GetKeyPressing(DIK_V))
	{
		_is_idle = false;
		_new_mesh_num = 2;
		_meshs[0]->SetAnimationIndex(_new_mesh_num);
		_meshs[_current_mesh_num]->ResetAnimation();
		_current_mesh_num = 2;
	}

	return GameObject::Tick(timeDelta);
}

int32_t Fittingdoll::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	auto mesh = _meshs[0];
	mesh->PlayAnimation(timeDelta);
	mesh = _meshs[_current_mesh_num];
	mesh->PlayAnimation(timeDelta);
	return GameObject::LateTick(timeDelta);
}

HRESULT Fittingdoll::Render()
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);

	auto mesh = _meshs[0];

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

auto Fittingdoll::GetCurrentDynamicMesh() -> std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>
{
	return std::make_pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>(static_cast<std::shared_ptr<MeshDynamic>>(_meshs[0]), static_cast<std::shared_ptr<MeshDynamic>>(_meshs[_current_mesh_num]));
}

auto Fittingdoll::AddComponents() -> HRESULT
{
	/* Com_Transform */
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 5.0f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;

	_transform_com->SetState(Transform::kState::kStatePosition, _float3(-622.779358f, 1064.66284f, -16.07339f));


	const auto animationNames = DataReaderManager::GetInstance().AllAnimationName();

	std::shared_ptr<MeshDynamic> mesh;
	for (auto animation : animationNames)
	{
		std::wstring prototypeName(TEXT("Prototype_Mesh_Ani_"));
		if (!_info.sex)
		{
			prototypeName.append(L"F_");
		}
		auto aniName = animation->animation_name;
		prototypeName.append(StringUtils::ConvertCtoW(aniName.c_str()));
		aniName.append(".x");
		if (FAILED(AddComponent(kScene::kSceneStatic, prototypeName, StringUtils::ConvertCtoW(aniName.c_str()), reinterpret_cast<std::shared_ptr<Component>*>(&mesh), &animation)))
			return E_FAIL;

		_meshs.push_back(mesh);
	}

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;


	for (auto& mesh : _meshs)
	{
		if (_meshs[0] != mesh)
		{
			LPD3DXANIMATIONSET		pAS = nullptr;
			mesh->GetAnimation()->GetAnimationController()->GetAnimationSet(0, &pAS);
			if (FAILED(_meshs[0]->GetAnimation()->GetAnimationController()->RegisterAnimationSet(pAS)))
			{
				std::cout << "애니메이션 추가도중 실패함" << std::endl;
			}
			pAS->Release();
		}
	}
	return S_OK;
}

auto Fittingdoll::SetUpConstantTable() const -> HRESULT
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

auto Fittingdoll::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Fittingdoll>
{
	auto instance = std::make_shared<Fittingdoll>(device);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create Fittingdoll");
		return nullptr;
	}
	return instance;
}

auto Fittingdoll::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<Fittingdoll>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Fittingdoll");
		return nullptr;
	}
	return instance;
}
