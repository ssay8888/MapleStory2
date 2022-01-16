#include "c_pch.h"
#include "inventory_ui.h"

#include "inventory_tab_btn/inventory_tab_btn.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

//static float closeX = 208.f;
//static float closeY = 237.f;
Inventory::Inventory() :
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
	_is_show = false;
	_moving = false;
}

HRESULT Inventory::NativeConstructPrototype()
{

	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_pos = { g_WinCX / 2.f, g_WinCY / 2.f, 0 };
	return GameObject::NativeConstructPrototype();
}

HRESULT Inventory::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	return GameObject::NativeConstruct(arg);
}

int32_t Inventory::Tick(const double timeDelta)
{
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_T))
	//{
	//	closeX += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_Y))
	//{
	//	closeX -= 1;
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_G))
	//{
	//	closeY += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_H))
	//{
	//	closeY -= 1;
	//}
	if (!_is_show)
	{
		return S_OK;
	}
	_input_device->InvalidateInputDevice();
	_is_lbutton_down = _input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton);
	_is_lbutton_up = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton);
	if (FAILED(CloseButtonTick(timeDelta)))
	{
		return E_FAIL;
	}
	if (FAILED(MoveInventoryFrame()))
	{
		return E_FAIL;
	}
	if (FAILED(TabButtonTick(timeDelta)))
	{
		return E_FAIL;
	}
	return GameObject::Tick(timeDelta);
}

int32_t Inventory::LateTick(const double timeDelta)
{
	if (!_is_show)
	{
		return S_OK;
	}
	for (int i = 1; i < Protocol::kInventoryType::kInventoryEnd; ++i)
	{
		auto tabBtn = _inventory_tab_btn[i];

		if (tabBtn)
		{
			tabBtn->LateTick(timeDelta);
		}
	}
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT Inventory::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_pos.y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _inventory_frame, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = 24;
	transformMatrix._22 = 24;
	transformMatrix._41 = (_pos.x + 208) + -(g_WinCX >> 1);
	transformMatrix._42 = -(_pos.y - 237) + (g_WinCY >> 1);
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _inventory_close_btn, _close_btn_over);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();
	for (int i = 1; i < Protocol::kInventoryType::kInventoryEnd; ++i)
	{
		auto tabBtn = _inventory_tab_btn[i];
		
		if (tabBtn)
		{
			tabBtn->Render(_float3(_pos.x - (g_WinCX >> 1), -_pos.y + (g_WinCY >> 1), 0.f), _shader_com);
		}
	}
	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto Inventory::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<Inventory>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		std::cout << "Inventory Clone Failed" << std::endl;
		return nullptr;
	}
	return instance;
}

auto Inventory::GetInventoryTab(const Protocol::kInventoryType type) -> std::shared_ptr<InventoryTabBtn>
{
	return _inventory_tab_btn[type];
}

auto Inventory::ChangeShow() -> void
{
	_is_show = !_is_show;
}

auto Inventory::Create() -> std::shared_ptr<Inventory>
{
	auto instance = std::make_shared<Inventory>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		std::cout << "Inventory Create Failed" << std::endl;
		return nullptr;
	}
	return instance;
}

auto Inventory::AddComponents() -> HRESULT
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Inventory_Frame"),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_inventory_frame))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Inventory_Close_Btn"),
		TEXT("Com_Texture_2"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_inventory_close_btn))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	for (int i = 0; i < Protocol::kInventoryType::kInventoryEnd; ++i)
	{
		auto btn = InventoryTabBtn::Create(static_cast<Protocol::kInventoryType>(i), _float3(-165.f, 217.f - (i * 24), 0));
		if (btn == nullptr)
		{
			return E_FAIL;
		}
		_inventory_tab_btn[i] = btn;
	}

	return S_OK;
}

auto Inventory::TabButtonTick(const double timeDelta) -> HRESULT
{
	std::shared_ptr<InventoryTabBtn> selectBtn = nullptr;
	for (int i = 1; i < Protocol::kInventoryType::kInventoryEnd; ++i)
	{
		auto tabBtn = _inventory_tab_btn[i];

		if (tabBtn)
		{
			tabBtn->SetParentPos(_float3(_pos.x - (g_WinCX >> 1), -_pos.y + (g_WinCY >> 1), 0));
			if (tabBtn->SelectBtn(_is_lbutton_down))
			{
				tabBtn->SetSelect(true);
				selectBtn = tabBtn;
			}
			tabBtn->Tick(_float3(_pos.x - (g_WinCX >> 1), -_pos.y + (g_WinCY >> 1), 0.f), timeDelta);
		}
	}
	if (selectBtn)
	{
		for (int i = 1; i < Protocol::kInventoryType::kInventoryEnd; ++i)
		{
			auto tabBtn = _inventory_tab_btn[i];

			if (tabBtn != nullptr && tabBtn != selectBtn)
			{
				tabBtn->SetSelect(false);
			}
		}
	}
	return S_OK;
}

auto Inventory::CloseButtonTick(double timeDelta) -> HRESULT
{
	_close_btn_over = false;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>((_pos.x + 208) - 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 237) - 24 * 0.5f),
	   static_cast<LONG>((_pos.x + 208) + 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 237) + 24 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		_close_btn_over = true;
		if (_is_lbutton_down)
		{
			_is_show = !_is_show;
		}
	}
	return S_OK;
}

auto Inventory::MoveInventoryFrame() -> HRESULT
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>(_pos.x - _size_x * 0.5f),
	   static_cast<LONG>(_pos.y - _size_y * 0.5f),
	   static_cast<LONG>(_pos.x + _size_x * 0.5f),
	   static_cast<LONG>(_pos.y - ((_size_y * 0.5f) - 50))
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		if (_is_lbutton_down)
		{
			_moving = true;
		}
	}
	if (_moving)
	{
		if (_is_lbutton_up)
		{
			_moving = false;
		}

		auto x = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kX);
		auto y = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kY);

		_pos.x += x;
		_pos.y += y;
	}
	return S_OK;
}
