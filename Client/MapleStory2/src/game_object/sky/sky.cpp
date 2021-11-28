#include "pch.h"
#include "sky.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_cube/vi_buffer_cube.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/pipe_line/pipe_line.h"

Sky::Sky(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Sky::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Sky::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (FAILED(AddComponents()))
		return E_FAIL;

	return S_OK;
}

int32_t Sky::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t Sky::LateTick(const double timeDelta)
{
	GameObject::LateTick(timeDelta);

	const _matrix viewMatrix = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);

	_matrix	camWorldMatrix = *D3DXMatrixInverse(&camWorldMatrix, nullptr, &viewMatrix);


	_transform_com->SetState(Transform::kState::kStatePosition, *reinterpret_cast<_float3*>(&camWorldMatrix.m[3][0]));

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderPriority, shared_from_this());

	return S_OK;
}

HRESULT Sky::Render()
{
	auto& pipeInstance = PipeLine::GetInstance();
	_graphic_device->SetTransform(D3DTS_WORLD, &_transform_com->GetWorldMatrix());

	const auto viewMatrix = pipeInstance.GetTransform(D3DTS_VIEW);
	_graphic_device->SetTransform(D3DTS_VIEW, &viewMatrix);
	const auto projectionMatrix = pipeInstance.GetTransform(D3DTS_PROJECTION);
	_graphic_device->SetTransform(D3DTS_PROJECTION, &projectionMatrix);

	_graphic_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	_graphic_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	_graphic_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	  
	_texture_com->SetUpOnGraphicDevice(0, 3);

	_vi_buffer_com->RenderViBuffer();

	_graphic_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	_graphic_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	_graphic_device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return GameObject::Render();
}

auto Sky::AddComponents() -> HRESULT
{
	/* Com_Transform*/
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 2.f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic), TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Sky"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	return S_OK;
}

auto Sky::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Sky>
{
	auto pInstance = std::make_shared<Sky>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating CSky");
		return nullptr;
	}
	return pInstance;
}

auto Sky::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<Sky>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CSky");
		return nullptr;
	}
	return pInstance;
}
