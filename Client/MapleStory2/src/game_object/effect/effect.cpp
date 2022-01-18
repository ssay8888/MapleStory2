#include "c_pch.h"
#include "effect.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/pipe_line/pipe_line.h"

auto Effect::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto Effect::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(GameObject::NativeConstruct(arg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	_transform_com->SetState(Transform::kState::kStatePosition, _float3(static_cast<float>(rand() % 10), 0.f, static_cast<float>(rand() % 10)));
	return S_OK;
}

auto Effect::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto Effect::LateTick(const double timeDelta) -> HRESULT
{
	_matrix			viewMatrix = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);

	D3DXMatrixInverse(&viewMatrix, nullptr, &viewMatrix);

	_transform_com->SetState(Transform::kState::kStatePosition, *reinterpret_cast<_float3*>(&viewMatrix.m[0][0]));
	_transform_com->SetState(Transform::kState::kStateLook, *reinterpret_cast<_float3*>(&viewMatrix.m[2][0]));


	ComputeViewZ(_transform_com);

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderAlpha, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto Effect::Render() -> HRESULT
{
	GameObject::Render();
	_graphic_device->SetTransform(D3DTS_WORLD, &_transform_com->GetWorldMatrix());
	const auto view = PipeLine::GetInstance().GetTransform(D3DTS_VIEW);
	const auto proj = PipeLine::GetInstance().GetTransform(D3DTS_PROJECTION);

	_graphic_device->SetTransform(D3DTS_VIEW, &view);
	_graphic_device->SetTransform(D3DTS_PROJECTION, &proj);

	_graphic_device->SetRenderState(D3DRS_LIGHTING, FALSE);

	_graphic_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_graphic_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_graphic_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//SRCCOLOR: 내가 그릴려고하는 색상;		
	//DESTCOLOR: 이미 그려져있던 색상;
	//
	//(SRCCOLOR.RGB) * SRCOLOR.A + (DESTCOLOR.RGB) * (1.f - SRCOLOR.A)

	/* 깊이테스트 이전에 수행. */
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, /*0 ~ 255*/1);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	_texture_com->SetUpOnGraphicDevice(0, static_cast<uint32_t>(m_fFrame));
	_vi_buffer_com->RenderViBuffer();

	_graphic_device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	_graphic_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	return S_OK;
}

auto Effect::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<Effect>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Effect");
		return nullptr;
	}
	return pInstance;
}

auto Effect::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Effect>
{
	auto pInstance = std::make_shared<Effect>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Effect");
		return nullptr;
	}
	return pInstance;
}

auto Effect::AddComponents() -> HRESULT
{

	/* Com_Transform */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com))))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Effect"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	/* Com_VIBuffer*/
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	return S_OK;
}
