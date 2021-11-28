#include "pch.h"
#include "back_ground.h"

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
	GameObject::Render();
	_matrix		identityMatrix, scaleMatrix;

	D3DXMatrixScaling(&scaleMatrix, 2.f, 2.f, 1.f);
	D3DXMatrixIdentity(&identityMatrix);

	/* 쉐이더 전역변수로 값을 전달한다. */

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &scaleMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &identityMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &identityMatrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	result = _shader_com->EndShader();

	return S_OK;
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
