#include "c_pch.h"
#include "character_beauty_ui.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterBeautyUi::CharacterBeautyUi(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT CharacterBeautyUi::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterBeautyUi::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	_info.size = _float3(461-100, 800, 0);
	_info.pos = _float3(g_WinCX - _info.size.x / 2, g_WinCY - _info.size.y * 0.5f, 0);

	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t CharacterBeautyUi::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t CharacterBeautyUi::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT CharacterBeautyUi::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _info.size.x;
	transformMatrix._22 = _info.size.y;
	transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_info.pos.y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);

	_vi_buffer_com->RenderViBuffer();

	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto CharacterBeautyUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<CharacterBeautyUi>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CharacterBeautyUi");
		return nullptr;
	}
	return pInstance;
}

auto CharacterBeautyUi::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CharacterBeautyUi>
{
	auto pInstance = std::make_shared<CharacterBeautyUi>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create CharacterBeautyUi");
		return nullptr;
	}
	return pInstance;
}

auto CharacterBeautyUi::AddComponents() -> HRESULT
{
	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyFrame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}
