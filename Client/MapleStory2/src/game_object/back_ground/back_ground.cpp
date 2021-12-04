#include "c_pch.h"
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

	float _size_x = 1280;
	float _size_y = 720;
	float _x = 1280 >> 1;
	float _y = 720 - _size_y * 0.5f;
	_matrix _proj_matrix;
	D3DXMatrixOrthoLH(&_proj_matrix, 1280, 720, 0.f, 1.f);


	_matrix			transformMatrix, viewMatrix, projMatrix;

	D3DXMatrixIdentity(&transformMatrix);

	/* 로컬스페이스 기준으로 크기변환과 위치변환만 셋팅한다. */
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;

	transformMatrix._41 = _x - (1280 >> 1);
	transformMatrix._42 = -_y + (720 >> 1);

	_graphic_device->SetTransform(D3DTS_WORLD, &transformMatrix);

	/* 뷰변환행렬. */
	_graphic_device->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&viewMatrix));

	/* 투영변환행렬 */
	_graphic_device->SetTransform(D3DTS_PROJECTION, &_proj_matrix);

	_graphic_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	_graphic_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	_graphic_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	_texture_com->SetUpOnGraphicDevice(0, 0);

	_vi_buffer_com->RenderViBuffer();

	_graphic_device->SetRenderState(D3DRS_LIGHTING, TRUE);


	//_matrix			transformMatrix, viewMatrix, projMatrix;
	//D3DXMatrixOrthoLH(&projMatrix, 1280, 720, 0.f, 1.f);
	//D3DXMatrixIdentity(&transformMatrix);
	//D3DXMatrixIdentity(&viewMatrix);
	//transformMatrix._11 = 1280;
	//transformMatrix._22 = 720;
	//transformMatrix._41 = (1280>>1) - (1280 >> 1);
	//transformMatrix._42 = -(720 - (720 * 0.5f)) + (720 >> 1);

	//auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpConstantTable("g_ProjMatrix", &projMatrix, sizeof(_matrix));
	//result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	//result = _shader_com->BeginShader(0);
	//_vi_buffer_com->RenderViBuffer();
	//result = _shader_com->EndShader();

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
		L"Prototype_Texture_Login_Logo",
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
