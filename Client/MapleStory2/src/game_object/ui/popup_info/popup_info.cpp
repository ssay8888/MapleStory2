#include "c_pch.h"
#include "popup_info.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

PopupInfo::PopupInfo():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT PopupInfo::NativeConstructPrototype()
{
	if (FAILED(AddComponents()))
	{
		return S_OK;
	}
	_size_x = 238;
	_size_y = 176;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstructPrototype();
}

HRESULT PopupInfo::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

int32_t PopupInfo::Tick(const double timeDelta)
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	_pos.x = ptMouse.x - centerX;
	_pos.y = -ptMouse.y + centerY;
	return GameObject::Tick(timeDelta);
}

int32_t PopupInfo::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT PopupInfo::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _pos.x - _size_x * 0.5f;
	transformMatrix._42 = _pos.y;


	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = 48;
	transformMatrix._22 = 48;
	transformMatrix._41 = _pos.x - (_size_x - 60) - 26;
	transformMatrix._42 = _pos.y + (48 / 2) + 10;
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _item_texture_com, 0);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	result = _shader_com->EndShader();


	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(centerX + (_pos.x - _size_x * 0.5f) - _size_x / 2 + 8),
	   static_cast<LONG>(centerY - (_pos.y)),
	   static_cast<LONG>(centerX + (_pos.x - _size_x * 0.5f) + _size_x / 2 -4),
	   static_cast<LONG>(centerY - (_pos.y) + 150)
	};

	std::wstring str;
	if (_info.str > 0)
	{
		str.append(L"STR : ").append(std::to_wstring(_info.str)).append(L"\r\n");
	}
	if (_info.dex > 0)
	{
		str.append(L"DEX : ").append(std::to_wstring(_info.dex)).append(L"\r\n");
	}
	if (_info.int_ > 0)
	{
		str.append(L"INT : ").append(std::to_wstring(_info.int_)).append(L"\r\n");
	}
	if (_info.luk > 0)
	{
		str.append(L"LUK : ").append(std::to_wstring(_info.luk)).append(L"\r\n");
	}
	if (_info.wap > 0)
	{
		str.append(L"°ø°Ý·Â : ").append(std::to_wstring(_info.wap)).append(L"\r\n");
	}
	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI,  DT_LEFT | DT_WORDBREAK, D3DCOLOR_ARGB(255, 255, 255, 255));
	return GameObject::Render();
}

auto PopupInfo::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<PopupInfo>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto PopupInfo::SetItemTexture(std::shared_ptr<Texture> itemTexture) -> void
{
	_item_texture_com = itemTexture;
}

auto PopupInfo::SetItemInfo(Item::ItemInfo itemInfo) -> void
{
	_info = itemInfo;
}

auto PopupInfo::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_ItemInfoPopup"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto PopupInfo::Create() -> std::shared_ptr<PopupInfo>
{
	auto instance = std::make_shared<PopupInfo>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}
