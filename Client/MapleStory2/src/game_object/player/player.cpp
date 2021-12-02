#include "pch.h"
#include "player.h"

#include <iostream>
#include <ostream>

#include "client_defines.h"
#include "resource.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/picking/picking.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/components/vi_buffer/vi_buffer_terrain/vi_buffer_terrain.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

Player::Player(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Player::NativeConstructPrototype()
{
	GameObject::NativeConstructPrototype();
	return S_OK;
}

HRESULT Player::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);

	if (FAILED(AddComponents()))
		return E_FAIL;

	_transform_com->SetScale(0.01f, 0.01f, 0.01f);
	return S_OK;
}

int32_t Player::Tick(const double timeDelta)
{
	GameObject::Tick(timeDelta);

	if (GetKeyState(VK_UP) & 0x8000)
	{
		_transform_com->GoStraight(timeDelta);
	}

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		_transform_com->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta);
	}
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		_transform_com->RotationAxis(_float3(0.f, 1.f, 0.f), timeDelta * -1.f);
	}

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		_transform_com->BackStraight(timeDelta);
	}
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		SetFocus(g_hEdit);
		wchar_t* a = new wchar_t[100];
		GetWindowText(g_hEdit, a, 100);
		std::wcout.imbue(std::locale("kor"));
		std::wcout << a << std::endl;
		SetWindowText(g_hEdit, L"");
		delete[] a;
	}
	if (GetKeyState('P') & 0x8000)
	{
		SetFocus(0);
	}

	
	const auto& object_manager = ObjectManager::GetInstance();
	const std::shared_ptr<ViBufferTerrain> viBuffer = std::static_pointer_cast<ViBufferTerrain>(
		object_manager.GetComponentPtr(
		static_cast<uint32_t>(kScene::kSceneGamePlay0), 
			L"Layer_BackGround", 
			L"Com_VIBuffer"));


	const std::shared_ptr<Transform> transform = std::static_pointer_cast<Transform>(object_manager.GetComponentPtr(static_cast<uint32_t>(kScene::kSceneGamePlay0),
	                                                                                                                 L"Layer_BackGround",
	                                                                                                                 L"Com_Transform"));

	_transform_com->StandOnTerrain(viBuffer, &transform->GetWorldMatrix());


	auto& picking = Picking::GetInstance();

	if (GetKeyState(VK_LBUTTON) & 0x8000)
	{
		const _float3* pTargetPos = picking.ComputePickingPoint(viBuffer, transform->GetWorldMatrix());

		if (nullptr != pTargetPos)
		{
			_target_pos = *pTargetPos;
			_transform_com->SetMovingState(true);
		}
	}

	_transform_com->ChaseTarget(_target_pos, timeDelta);
	//if (GetKeyState(VK_RBUTTON) & 0x8000)
	//{
	//	const _float3*	vPickingPos = picking.ComputePickingPoint(_vi_buffer_com, _transform_com->GetWorldMatrix());
	//}


	return GameObject::Tick(timeDelta);
}

int32_t Player::LateTick(const double timeDelta)
{
	GameObject::LateTick(timeDelta);
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return S_OK;
}

HRESULT Player::Render()
{

	GameObject::Render();

	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	const uint32_t dwNumMaterials = _mesh_com->GetNumMaterials();

	auto result = _shader_com->BeginShader(0);

	for (uint32_t i = 0; i < dwNumMaterials; ++i)
	{
		if (FAILED(_mesh_com->SetUpTextureOnShader(_shader_com, "g_DiffuseTexture", MeshMaterialTexture::kType::kTypeDiffuse, i)))
			return E_FAIL;

		_shader_com->Commit();

		if (FAILED(_mesh_com->Render(i)))
			return E_FAIL;
	}

	result = _shader_com->EndShader();

	return GameObject::Render();
}

auto Player::AddComponents() -> HRESULT
{
	/* Com_Transform */
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 5.0f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(AddComponent(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;

	if (FAILED(AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Mesh_Man"), TEXT("Com_Mesh"), reinterpret_cast<std::shared_ptr<Component>*>(&_mesh_com))))
		return E_FAIL;

	if (FAILED(AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;
	
	return S_OK;
}

auto Player::SetUpConstantTable() const -> HRESULT
{
	auto& pipeline = PipeLine::GetInstance();
	const auto view = pipeline.GetTransform(D3DTS_VIEW);
	const auto project = pipeline.GetTransform(D3DTS_PROJECTION);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", _transform_com->GetWorldMatrix(), sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &project, sizeof(_matrix));
	return S_OK;
}

auto Player::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Player>
{
	std::shared_ptr<Player> instance = std::make_shared<Player>(device);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Player");
		return nullptr;
	}
	return instance;
}

auto Player::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	std::shared_ptr<Player> instance = std::make_shared<Player>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Player");
		return nullptr;
	}
	return instance;
}
