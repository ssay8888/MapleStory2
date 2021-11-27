#include "pch.h"
#include "back_ground.h"

#include "client_defines.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

BackGround::BackGround(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT BackGround::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT BackGround::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	

	if (FAILED(AddComponents()))
		return E_FAIL;

	_transform_com->SetScale(3.f, 3.f, 3.f);

	return S_OK;
}

int32_t BackGround::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t BackGround::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderPriority, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT BackGround::Render()
{
	//GameObject::Render();
	//_matrix		identityMatrix, scaleMatrix;

	//D3DXMatrixScaling(&scaleMatrix, 2.f, 2.f, 1.f);
	//D3DXMatrixIdentity(&identityMatrix);

	///* 쉐이더 전역변수로 값을 전달한다. */

	//auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &scaleMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpConstantTable("g_ViewMatrix", &identityMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpConstantTable("g_ProjMatrix", &identityMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	///*LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	//LPDIRECT3DBASETEXTURE9 pTexture = m_pTextureCom->GetTexture();

	//pEffect->SetTexture()*/

	///* Shader_Rect 쉐이더로 그린다. */

	//result = _shader_com->BeginShader(0);

	//_vi_buffer_com->RenderViBuffer();

	//result = _shader_com->EndShader();
	GameObject::Render();

	if (FAILED(_texture_com->SetUpOnGraphicDevice(0)))
		return E_FAIL;

	_matrix		IdentityMatrix, ScaleMatrix;

	D3DXMatrixScaling(&ScaleMatrix, 2.f, 2.f, 1.f);
	D3DXMatrixIdentity(&IdentityMatrix);
	if (FAILED(_graphic_device->SetTransform(D3DTS_WORLD, &ScaleMatrix)))
	{
		return int32_t();
	}
	if (FAILED(_graphic_device->SetTransform(D3DTS_VIEW, &IdentityMatrix)))
	{
		return int32_t();
	}
	if (FAILED(_graphic_device->SetTransform(D3DTS_PROJECTION, &IdentityMatrix)))
	{
		return int32_t();
	}
	_vi_buffer_com->RenderViBuffer();
	return int32_t();
}

auto BackGround::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<BackGround>
{
	auto pInstance = std::make_shared<BackGround>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating BackGround");
		return nullptr;
	}
	return pInstance;
}

auto BackGround::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<BackGround>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone BackGround");
		return nullptr;
	}
	return pInstance;
}

auto BackGround::AddComponents() -> HRESULT
{

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic),
		L"Prototype_Transform",
		L"Com_Transform",
		reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic),
		L"Prototype_VIBuffer_Rect",
		L"Com_VIBuffer",
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic),
		L"Prototype_Texture_Default",
		L"Com_Texture",
		reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic),
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}
