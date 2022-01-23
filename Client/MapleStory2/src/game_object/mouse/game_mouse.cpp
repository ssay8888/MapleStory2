#include "c_pch.h"
#include "game_mouse.h"

#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

GameMouse::GameMouse():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT GameMouse::NativeConstructPrototype()
{
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstructPrototype();
}

HRESULT GameMouse::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	ShowCursor(false);
	return GameObject::NativeConstruct(arg);
}

HRESULT GameMouse::Tick(const double timeDelta)
{
	_input_device->InvalidateInputDevice();
	_is_click = false;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	_pos = {static_cast<float>(ptMouse.x), static_cast<float>(ptMouse.y), 0 };

	if (_input_device->GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
	{
		_is_click = true;
	}

	return GameObject::Tick(timeDelta);
}

HRESULT GameMouse::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderMouse, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT GameMouse::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 45;
	transformMatrix._22 = 45;
	transformMatrix._41 = (_pos.x +  (45 / 2)) + -(g_WinCX >> 1);
	transformMatrix._42 = -(_pos.y + (45 / 2)) + (g_WinCY >> 1);
	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _click_mouse, _is_click);
	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto GameMouse::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<GameMouse>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto GameMouse::Create() -> std::shared_ptr<GameMouse>
{
	auto instance = std::make_shared<GameMouse>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto GameMouse::AddComponents() -> HRESULT
{
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Texture_Click_Mouse"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_click_mouse))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Texture_Grab_Mouse"), TEXT("Com_Texture_2"), reinterpret_cast<std::shared_ptr<Component>*>(&_grab_mouse))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Shader_Rect"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	return S_OK;
}
