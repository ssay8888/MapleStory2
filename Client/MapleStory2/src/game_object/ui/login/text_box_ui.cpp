#include "c_pch.h"
#include "text_box_ui.h"

#include <iostream>

#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

TextBoxUi::TextBoxUi(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT TextBoxUi::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT TextBoxUi::NativeConstruct(void* arg)
{
	if (arg == nullptr || FAILED(AddComponents()))
	{
		return E_FAIL;
	}

	const auto information = static_cast<TextBoxInformation*>(arg);

	if (FAILED(D3DXCreateFontIndirectW(_graphic_device.Get(), &information->font_info, &_font)))
	{
		MSGBOX("Creating Failed TextBoxUi");
		return E_FAIL;
	}
	_size = information->size;
	_pos = information->pos;
	_is_password = information->is_password;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_max_content_length = std::max(information->max_content_length, 13);
	_rc = { static_cast<LONG>(_pos.x - (_size.x / 2)), static_cast<LONG>(_pos.y - (_size.y / 2)), static_cast<LONG>(_pos.x + (_size.x / 2)), static_cast<LONG>(_pos.y + (_size.y / 2)) };
	return GameObject::NativeConstruct(arg);
}

int32_t TextBoxUi::Tick(const double timeDelta)
{
	if (InputDevice::GetInstance().GetDirectMouseKeyState(InputDevice::kDirectInMouseButton::kLeftButton))
	{
		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_Wnd, &ptMouse);

		RECT		rcUI = {
			static_cast<LONG>(_pos.x - _size.x * 0.5f),
			static_cast<LONG>(_pos.y - _size.y * 0.5f),
			static_cast<LONG>(_pos.x + _size.x * 0.5f),
			static_cast<LONG>(_pos.y + _size.y * 0.5f)
		};


		if (PtInRect(&rcUI, ptMouse))
		{
			SetFocus(true);
		}
		else
		{
			SetFocus(false);
		}
	}
	if (IsFocus())
	{
		wchar_t str[100];
		GetWindowText(g_hEdit, str, 100);
		SetText(str);
	}
	return GameObject::Tick(timeDelta);
}

int32_t TextBoxUi::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT TextBoxUi::Render()
{
	_matrix			TransformMatrix, ViewMatrix, ProjMatrix;

	D3DXMatrixIdentity(&TransformMatrix);

	/* 로컬스페이스 기준으로 크기변환과 위치변환만 셋팅한다. */
	TransformMatrix._11 = _size.x;
	TransformMatrix._22 = _size.y;

	TransformMatrix._41 = _pos.x - (g_WinCX >> 1);
	TransformMatrix._42 = -_pos.y + (g_WinCY >> 1);

	D3DXMatrixIdentity(&ViewMatrix);
	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &TransformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &ViewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);

	_vi_buffer_com->RenderViBuffer();

	result = _shader_com->EndShader();
	UiDrawText();

	return GameObject::Render();
}

auto TextBoxUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<TextBoxUi>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone TextBoxUi");
		return nullptr;
	}

	return instance;
}

auto TextBoxUi::Create() -> std::shared_ptr<TextBoxUi>
{
	auto instance = std::make_shared<TextBoxUi>(GraphicDevice::GetInstance().GetDevice());

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create TextBoxUi");
		return nullptr;
	}

	return instance;
}

auto TextBoxUi::ClearText() -> void
{
	_contents.clear();
}

auto TextBoxUi::AddText(const std::wstring& str) -> void
{
	_contents.append(str);
}

auto TextBoxUi::SetText(const std::wstring& str) -> void
{
	_contents = str;
}

auto TextBoxUi::GetText() const -> std::wstring
{
	return _contents;
}

auto TextBoxUi::SetFocus(const bool focus) -> void
{
	_is_focus = focus;
	if (_is_focus)
	{
		::SetFocus(g_hEdit);
		SetWindowText(g_hEdit, _contents.c_str());
		SendMessage(g_hEdit, EM_SETSEL, static_cast<WPARAM>(lstrlen(_contents.c_str())), static_cast<LPARAM>(lstrlen(_contents.c_str())));
	}
}

auto TextBoxUi::IsFocus() const -> bool
{
	return _is_focus;
}

auto TextBoxUi::UiDrawText() -> void
{
	if(_is_password)
	{
		std::wstring contents;
		for (int i = 0; i  < _contents.size(); ++i)
		{
			contents.append(L"*");
		}
		_font->DrawTextW(NULL, contents.c_str(), -1, &_rc, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
	else
	{
		_font->DrawTextW(NULL, _contents.c_str(), -1, &_rc, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
}

auto TextBoxUi::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic), 
		TEXT("Prototype_Transform"), 
		TEXT("Com_Transform"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic), 
		TEXT("Prototype_Texture_Default2"), 
		TEXT("Com_Texture"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic), 
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(static_cast<int32_t>(kScene::kSceneStatic),
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}
