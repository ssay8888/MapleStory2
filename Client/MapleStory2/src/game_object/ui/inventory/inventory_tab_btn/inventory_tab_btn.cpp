#include "c_pch.h"
#include "inventory_tab_btn.h"

#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

InventoryTabBtn::InventoryTabBtn(Protocol::kInventoryType type, _float3 pos) :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_type(type),
	_pos(pos)
{
}

HRESULT InventoryTabBtn::NativeConstructPrototype()
{
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	if (FAILED(AddComponents()))
	{
		return S_OK;
	}
	return GameObject::NativeConstructPrototype();
}

HRESULT InventoryTabBtn::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}
int32_t InventoryTabBtn::Tick(const double timeDelta)
{
	_input_device->InvalidateInputDevice();
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_T))
	//{
	//	_pos.x += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_Y))
	//{
	//	_pos.x -= 1;
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_G))
	//{
	//	_pos.y += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_H))
	//{
	//	_pos.y -= 1;
	//}

	return GameObject::Tick(timeDelta);
}

int32_t InventoryTabBtn::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT InventoryTabBtn::Render()
{
	return GameObject::Render();
}


auto InventoryTabBtn::Render(const _float3& pos, std::shared_ptr<Shader> shader) -> HRESULT
{
	if (_is_over || _is_click)
	{
		_matrix			transformMatrix, viewMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		D3DXMatrixIdentity(&viewMatrix);
		transformMatrix._11 = 115;
		transformMatrix._22 = 24;
		transformMatrix._41 = pos.x + _pos.x;
		transformMatrix._42 = pos.y + _pos.y;
		auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _inventory_tab_btn, _is_click ? 1 : 0);
		shader->Commit();
		_vi_buffer_com->RenderViBuffer();
	}


	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(centerX + (_parent_pos.x + _pos.x) - 115 * 0.5f),
	   static_cast<LONG>(centerY - (_parent_pos.y + _pos.y) - 24 * 0.5f),
	   static_cast<LONG>(centerX + (_parent_pos.x + _pos.x) + 115 * 0.5f),
	   static_cast<LONG>(centerY - (_parent_pos.y + _pos.y) + 24 * 0.5f)
	};
	std::wstring str;
	switch (_type)
	{
	case Protocol::kInventoryEqp:
		str.append(L"장비");
		break;
	case Protocol::kInventoryEtc:
		str.append(L"기타");
		break;
	default: ;
	}
	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI, DT_VCENTER | DT_CENTER, D3DCOLOR_ARGB(255, 0, 0, 0));
	return S_OK;
}

auto InventoryTabBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<InventoryTabBtn>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		std::cout << "Inventory Clone Failed" << std::endl;
		return nullptr;
	}
	return instance;
}

auto InventoryTabBtn::SelectBtn() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	const RECT		rcUI = {
	   static_cast<LONG>(centerX + (_parent_pos.x + _pos.x) - 115 * 0.5f),
	   static_cast<LONG>(centerY - (_parent_pos.y + _pos.y) - 24 * 0.5f),
	   static_cast<LONG>(centerX + (_parent_pos.x + _pos.x) + 115 * 0.5f),
	   static_cast<LONG>(centerY - (_parent_pos.y + _pos.y) + 24 * 0.5f)
	};
	_is_over = false;
	if (PtInRect(&rcUI, ptMouse))
	{
		_is_over = true;
		if (_input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			return true;
		}
	}
	return false;
}

auto InventoryTabBtn::SetParentPos(const _float3 pos) -> void
{
	_parent_pos = pos;
}

auto InventoryTabBtn::SetSelect(bool click)->void
{
	_is_click = click;
}

auto InventoryTabBtn::IsSelect() -> bool
{
	return _is_click;
}

auto InventoryTabBtn::Create(Protocol::kInventoryType type, _float3 pos) -> std::shared_ptr<InventoryTabBtn>
{
	auto instance = std::make_shared<InventoryTabBtn>(type, pos);
	if (FAILED(instance->NativeConstructPrototype()))
	{
		std::cout << "Inventory Create Failed" << std::endl;
		return nullptr;
	}
	return instance;
}

auto InventoryTabBtn::AddComponents() -> HRESULT
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Inventory_Tab_Btn"),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_inventory_tab_btn))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	return S_OK;
}
