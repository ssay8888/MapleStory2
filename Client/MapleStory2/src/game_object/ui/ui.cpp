#include "c_pch.h"
#include "ui.h"

#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

Ui::Ui(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device),
	_x(0),
	_y(0),
	_size_x(461),
	_size_y(730)
{
}
 
HRESULT Ui::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Ui::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;
	
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto Ui::Tick(const double timeDelta) -> HRESULT
{
	 GameObject::Tick(timeDelta);
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	 const RECT		rcUI = {
		static_cast<LONG>(_x - _size_x * 0.5f),
		static_cast<LONG>(_y - _size_y * 0.5f),
		static_cast<LONG>(_x + _size_x * 0.5f),
		static_cast<LONG>(_y + _size_y * 0.5f)
	};

	if (PtInRect(&rcUI, ptMouse))
		MSGBOX("유아이충돌.");
	return S_OK;
}

auto Ui::LateTick(const double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT Ui::Render()
{
	GameObject::Render();

	_matrix			transformMatrix, viewMatrix, projMatrix;

	D3DXMatrixIdentity(&transformMatrix);

	/* 로컬스페이스 기준으로 크기변환과 위치변환만 셋팅한다. */
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;

	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -_y + (g_WinCY >> 1);

	_graphic_device->SetTransform(D3DTS_WORLD, &transformMatrix);

	/* 뷰변환행렬. */
	_graphic_device->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&viewMatrix));

	/* 투영변환행렬 */
	_graphic_device->SetTransform(D3DTS_PROJECTION, &_proj_matrix);

	_graphic_device->SetRenderState(D3DRS_LIGHTING, FALSE);

	_texture_com->SetUpOnGraphicDevice(0, 0);

	_vi_buffer_com->RenderViBuffer();

	_graphic_device->SetRenderState(D3DRS_LIGHTING, TRUE);


	return S_OK;
}

auto Ui::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Ui>
{
	auto pInstance = std::make_shared<Ui>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating UI");
		return nullptr;
	}
	return pInstance;
}

auto Ui::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<Ui>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Creating UI");
		return nullptr;
	}
	return pInstance;
}

auto Ui::AddComponents() -> HRESULT
{

	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_Texture_Default2"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	return S_OK;
}
