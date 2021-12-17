#include "c_pch.h"
#include "character_create_name_popup.h"

#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterCreateNamePopup::CharacterCreateNamePopup():
	TextBoxUi(GraphicDevice::GetInstance().GetDevice()),
	_state()
{
}

HRESULT CharacterCreateNamePopup::NativeConstructPrototype()
{
	return TextBoxUi::NativeConstructPrototype();
}

HRESULT CharacterCreateNamePopup::NativeConstruct(void* arg)
{
	_info = *static_cast<CharacterCreateNamePopupInfo*>(arg);
	TextBoxUi::NativeConstruct(&_info.textbox_info);
	_rc = {
		static_cast<LONG>((_info.popup_pos.x - _info.popup_size.x * 0.5f) + _info.textbox_info.pos.x),
		static_cast<LONG>((_info.popup_pos.y - _info.popup_size.y * 0.5f) + _info.textbox_info.pos.y),
		static_cast<LONG>((_info.popup_pos.x + _info.popup_size.x * 0.5f) - _info.textbox_info.pos.x),
		static_cast<LONG>((_info.popup_pos.y + _info.popup_size.y * 0.5f) - _info.textbox_info.pos.y)
	};
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);

	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	return S_OK;
}

int32_t CharacterCreateNamePopup::Tick(double timeDelta)
{
	if (_is_show)
	{
		if (InputDevice::GetInstance().GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			POINT		ptMouse;
			GetCursorPos(&ptMouse);
			ScreenToClient(g_Wnd, &ptMouse);

			if (PtInRect(&_rc, ptMouse))
			{
				SetFocus(true);
				EnableWindow(g_hEdit, true);
			}
			else
			{
				SetFocus(false);
			}

			if (IsCancelCollision())
			{
				ChangeShow(false);
			}
		}
		if (IsFocus())
		{
			wchar_t str[100];
			GetWindowText(g_hEdit, str, 100);
			SetText(str);
		}

	}
	return S_OK;
}

int32_t CharacterCreateNamePopup::LateTick(double timeDelta)
{
	return TextBoxUi::LateTick(timeDelta);
}

HRESULT CharacterCreateNamePopup::Render()
{
	return TextBoxUi::Render();
}

auto CharacterCreateNamePopup::Render(std::shared_ptr<Shader> shader) -> HRESULT
{
	if (_is_show)
	{
		_matrix			transformMatrix, viewMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		D3DXMatrixIdentity(&viewMatrix);
		transformMatrix._11 = _info.popup_size.x;
		transformMatrix._22 = _info.popup_size.y;
		transformMatrix._41 = _info.popup_pos.x - (g_WinCX >> 1);
		transformMatrix._42 = -_info.popup_pos.y + (g_WinCY >> 1);


		auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture, 0);
		shader->Commit();

		_vi_buffer_com->RenderViBuffer();
		TextBoxUi::UiDrawText();
	}
	return S_OK;
}

std::shared_ptr<GameObject> CharacterCreateNamePopup::Clone(void* arg)
{
	return nullptr;
}

auto CharacterCreateNamePopup::Create(CharacterCreateNamePopupInfo* arg) -> std::shared_ptr<CharacterCreateNamePopup>
{
	auto pInstance = std::make_shared<CharacterCreateNamePopup>();

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Create CharacterCreateNamePopup");
		return nullptr;
	}

	return pInstance;
}

auto CharacterCreateNamePopup::IsCollision() const -> bool
{
	return false;
}

auto CharacterCreateNamePopup::GetButtonState() const -> kCreateJobBtnState
{
	return _state;
}

auto CharacterCreateNamePopup::IsShow() const -> bool
{
	return _is_show;
}

auto CharacterCreateNamePopup::ChangeShow(const bool show) -> void
{
	_is_show = show;
}

auto CharacterCreateNamePopup::AddComponents() -> HRESULT
{

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer_Name"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_CreateName"), TEXT("Com_NameCreate"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture))))
		return E_FAIL;

	return S_OK;
}

auto CharacterCreateNamePopup::IsOkCollision() const -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
		static_cast<LONG>((_info.popup_pos.x - _info.popup_size.x * 0.5f) + 52.f),
		static_cast<LONG>((_info.popup_pos.y - _info.popup_size.y * 0.5f) + 105.f),
		static_cast<LONG>((_info.popup_pos.x + _info.popup_size.x * 0.5f) - 124.f),
		static_cast<LONG>((_info.popup_pos.y + _info.popup_size.y * 0.5f) - 5)
	};

	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto CharacterCreateNamePopup::IsCancelCollision() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
		static_cast<LONG>((_info.popup_pos.x - _info.popup_size.x * 0.5f) + 125.f),
		static_cast<LONG>((_info.popup_pos.y - _info.popup_size.y * 0.5f) + 105.f),
		static_cast<LONG>((_info.popup_pos.x + _info.popup_size.x * 0.5f) - 50.f),
		static_cast<LONG>((_info.popup_pos.y + _info.popup_size.y * 0.5f) - 5)
	};

	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}
