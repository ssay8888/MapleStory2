#include "c_pch.h"
#include "character_select_ui.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterSelectUi::CharacterSelectUi(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device),
	_x(0),
	_y(0),
	_size_x(0),
	_size_y(0)
{
}

HRESULT CharacterSelectUi::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterSelectUi::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	_size_x = 461 - 100;
	_size_y = 720;
	_x = g_WinCX - _size_x / 2;
	_y = g_WinCY - _size_y * 0.5f;

	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t CharacterSelectUi::Tick(double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t CharacterSelectUi::LateTick(const double timeDelta)
{
	GameObject::Render();
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());

	return S_OK;
}

HRESULT CharacterSelectUi::Render()
{

	//_matrix			transformMatrix, viewMatrix, projMatrix;

	//D3DXMatrixIdentity(&transformMatrix);

	///* 로컬스페이스 기준으로 크기변환과 위치변환만 셋팅한다. */
	//transformMatrix._11 = _size_x;
	//transformMatrix._22 = _size_y;

	//transformMatrix._41 = _x - (g_WinCX >> 1);
	//transformMatrix._42 = -_y + (g_WinCY >> 1);

	//_graphic_device->SetTransform(D3DTS_WORLD, &transformMatrix);

	///* 뷰변환행렬. */
	//_graphic_device->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&viewMatrix));

	///* 투영변환행렬 */
	//_graphic_device->SetTransform(D3DTS_PROJECTION, &_proj_matrix);

	//_graphic_device->SetRenderState(D3DRS_LIGHTING, FALSE);

	//_texture_com->SetUpOnGraphicDevice(0, 0);

	//_vi_buffer_com->RenderViBuffer();

	//_graphic_device->SetRenderState(D3DRS_LIGHTING, TRUE);



	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -_y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto CharacterSelectUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<CharacterSelectUi>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone UI");
		return nullptr;
	}
	return pInstance;
}

auto CharacterSelectUi::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CharacterSelectUi>
{
	auto pInstance = std::make_shared<CharacterSelectUi>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create UI");
		return nullptr;
	}
	return pInstance;
}

auto CharacterSelectUi::AddComponents() -> HRESULT
{
	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectFrame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;


	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}
