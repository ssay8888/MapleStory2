#include "c_pch.h"
#include "Item.h"

#include "src/game_object/ui/ket_set_ui/key_set_manager.h"
#include "src/game_object/ui/ket_set_ui/key_set_ui.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"

Item::Item(const ItemInfo info):
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_info(info)
{
}

auto Item::NativeConstructPrototype() -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_original_pos.x += (_info.position % 6) * 50;
	_original_pos.y -= (_info.position / 6) * 50;
	_pos = _original_pos;
	return GameObject::NativeConstructPrototype();
}

auto Item::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}
auto Item::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto Item::Tick(const _float3& pos, const double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();
	if (_is_select_item)
	{
		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_Wnd, &ptMouse);

		const float centerX = g_WinCX >> 1;
		const float centerY = g_WinCY >> 1;
		_pos.x = ptMouse.x - centerX;
		_pos.y = -ptMouse.y + centerY;

	}
	return GameObject::Tick(timeDelta);
}

auto Item::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto Item::Render() -> HRESULT
{
	return GameObject::Render();
}

auto Item::Render(const _float3& pos, std::shared_ptr<Shader> shader) const -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 48;
	transformMatrix._22 = 48;
	if (_is_select_item)
	{
		transformMatrix._41 = _pos.x;
		transformMatrix._42 = _pos.y;
	}
	else
	{
		transformMatrix._41 = pos.x + _pos.x;
		transformMatrix._42 = pos.y + _pos.y;
	}
	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	if (_info.type != Protocol::kInventoryEqp && _info.type != Protocol::kInventoryEquipped)
	{
		const float centerX = g_WinCX >> 1;
		const float centerY = g_WinCY >> 1;
		RECT		rcUI = {
		   static_cast<LONG>(centerX + (pos.x + _original_pos.x) - 48 * 0.5f),
		   static_cast<LONG>(centerY - (pos.y + _original_pos.y) - 48 * 0.5f),
		   static_cast<LONG>(centerX + (pos.x + _original_pos.x) + 48 * 0.5f),
		   static_cast<LONG>(centerY - (pos.y + _original_pos.y) + 48 * 0.5f)
		};

		std::wstring str(std::to_wstring(GetItemQuantity()));
		GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI, DT_RIGHT, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	return S_OK;
}

auto Item::RenderEquipped(const _float3& pos, std::shared_ptr<Shader> shader) const -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 48;
	transformMatrix._22 = 48;
	transformMatrix._41 = pos.x;
	transformMatrix._42 = pos.y;
	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	return S_OK;
}

auto Item::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto Item::SearchMouseOverState(const _float3& pos) -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(centerX + (pos.x + _original_pos.x) - 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y + _original_pos.y) - 48 * 0.5f),
	   static_cast<LONG>(centerX + (pos.x + _original_pos.x) + 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y + _original_pos.y) + 48 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto Item::SearchMouseOverStateEquipped(const _float3& pos) -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(centerX + (pos.x) - 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y) - 48 * 0.5f),
	   static_cast<LONG>(centerX + (pos.x) + 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y) + 48 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto Item::SelectItem(const _float3& pos, uint8_t isLButtonDown, uint8_t isLButtonUp) const -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(centerX + (pos.x + _original_pos.x) - 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y + _original_pos.y) - 48 * 0.5f),
	   static_cast<LONG>(centerX + (pos.x + _original_pos.x) + 48 * 0.5f),
	   static_cast<LONG>(centerY - (pos.y + _original_pos.y) + 48 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		if (!_is_select_item && isLButtonDown)
		{
			return true;
		}
	}
	return _is_select_item;
}

auto Item::UnSelectItem(const _float3& pos, uint8_t isLButtonDown, uint8_t isLButtonUp) const -> bool
{
	if (_is_select_item)
	{
		if (isLButtonUp)
		{
			return true;
		}
	}
	return false;
}

auto Item::SetSelectItem(const bool select)->void
{
	_is_select_item = select;
}

auto Item::IsSelectItem() const -> bool
{
	return _is_select_item;
}

auto Item::MouseOverState() const -> bool
{
	return _is_up_mouse;
}

auto Item::SetIsMouseUp(const bool isup) -> void
{
	_is_up_mouse = isup;
}

auto Item::FindPosition(const _float3& pos) -> int32_t
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	_float3 originalPos{ -81, 154, 0 };

	for (int i = 0; i < 48; ++i)
	{
		originalPos.x = -81.f + ((i % 6) * 50);
		originalPos.y = 154.f - ((i / 6) * 50);
		RECT		rcUI = {
		   static_cast<LONG>(centerX + (pos.x + originalPos.x) - 48 * 0.5f),
		   static_cast<LONG>(centerY - (pos.y + originalPos.y) - 48 * 0.5f),
		   static_cast<LONG>(centerX + (pos.x + originalPos.x) + 48 * 0.5f),
		   static_cast<LONG>(centerY - (pos.y + originalPos.y) + 48 * 0.5f)
		};
		if (PtInRect(&rcUI, ptMouse))
		{
			return i;
		}
	}
	return -1;
}

auto Item::GetPosition() const -> int32_t
{
	return _info.position;
}

auto Item::ChangePosition(const int32_t position) -> void
{
	_original_pos = { -81, 154, 0 };
	_original_pos.x += (position % 6) * 50;
	_original_pos.y -= (position / 6) * 50;
	_pos = _original_pos;
	_info.position = position;
}

auto Item::ResetPosition() -> void
{
	_pos = _original_pos;
}

auto Item::GetPopupInfo() const -> std::shared_ptr<PopupInfo>
{
	return _popup_info_com;
}

auto Item::GetItemId() const -> int32_t
{
	return _info.item_id;
}

auto Item::GetItemQuantity() const -> int32_t
{
	return _info.quantity;
}

auto Item::SetItemQuantity(int32_t value) -> void
{
	_info.quantity = value;
}

auto Item::GetInventoryType() const -> Protocol::kInventoryType
{
	return _info.type;
}

auto Item::GetItemIcon() const -> std::shared_ptr<Texture>
{
	return _texture_com;
}

auto Item::Create(ItemInfo info) -> std::shared_ptr<Item>
{
	auto instance = std::make_shared<Item>(info);
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

HRESULT Item::AddComponents()
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);

	if (FAILED(AddComponent(kScene::kSceneStatic,
		fmt::format(L"Prototype_Texture_{0}", _info.item_id),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	_popup_info_com = PopupInfo::Create();
	_popup_info_com->SetItemTexture(_texture_com);
	return S_OK;
}