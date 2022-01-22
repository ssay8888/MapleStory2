#include "c_pch.h"
#include "equipped_ui.h"

#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/equipped/equipped.h"
#include "src/game_object/item/Item.h"
#include "src/game_object/player/player.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/inventory/inventory_tab_btn/inventory_tab_btn.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/managers/character_stat/character_stat.h"
#include "src/managers/sound_manager/sound_manager.h"
#include "src/managers/weapon_manager/weapon_manager.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "statup_btn/statup_btn.h"
#include "string_utils/string_utils.h"

EquippedUi::EquippedUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
	_is_show = false;
	_moving = false;
}

auto EquippedUi::NativeConstructPrototype() -> HRESULT
{

	return GameObject::NativeConstructPrototype();
}

auto EquippedUi::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_pos = { g_WinCX / 2.f, g_WinCY / 2.f, 0 };
	return GameObject::NativeConstruct(arg);
}

auto EquippedUi::Tick(const double timeDelta) -> HRESULT
{
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

	auto& objectManager = ObjectManager::GetInstance();
	const auto baseInventory = objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
	auto inventory = std::static_pointer_cast<Inventory>(baseInventory);
	if (inventory)
	{
		auto equippedTab = inventory->GetInventoryTab(Protocol::kInventoryEquipped);
		auto items = equippedTab->AllItem();
		for (auto& item : items)
		{
			if (item.second)
			{
				item.second->GetPopupInfo()->Tick(timeDelta);
			}
		}
		if (_input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kRightButton))
		{
			if (ColliderCoat())
			{
				UnEqpItem(equippedTab, inventory, GameContents::kEquipeType::kCoat);
			}
			if (ColliderPants())
			{
				UnEqpItem(equippedTab, inventory, GameContents::kEquipeType::kPants);
			}
			if (ColliderShoes())
			{
				UnEqpItem(equippedTab, inventory, GameContents::kEquipeType::kShoes);
			}
			if (ColliderWeapon())
			{
				UnEqpItem(equippedTab, inventory, GameContents::kEquipeType::kWeapon);
			}
			
		}
	}
	for (int i = 0; i < Protocol::kStatType::kStatEnd; ++i)
	{
		_statup_btn[i]->Tick(_float3(_pos.x + 230, _pos.y - 83 + (i * 21), 0), timeDelta);
	}
	return GameObject::Tick(timeDelta);
}

auto EquippedUi::LateTick(const double timeDelta) -> HRESULT
{
	if (!_is_show)
	{
		return S_OK;
	}
	for (int i = 0; i < Protocol::kStatType::kStatEnd; ++i)
	{
		_statup_btn[i]->LateTick(_float3(_pos.x + 230, _pos.y - 83 + (i * 21), 0), timeDelta);
	}
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto EquippedUi::Render() -> HRESULT
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
	transformMatrix._41 = (_pos.x + 220) + -(g_WinCX >> 1);
	transformMatrix._42 = -(_pos.y - 125) + (g_WinCY >> 1);
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _inventory_close_btn, _close_btn_over);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	ItemInfoRender();
	CharacterInfoRender();
	CharacterStatRender();
	for (int i = 0; i < Protocol::kStatType::kStatEnd; ++i)
	{
		_statup_btn[i]->Render(_float3(_pos.x + 230, _pos.y - 83 + (i * 21), 0), _shader_com);
	}
	result = _shader_com->EndShader();




	return GameObject::Render();
}

auto EquippedUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<EquippedUi>();
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto EquippedUi::Create() -> std::shared_ptr<EquippedUi>
{
	auto instance = std::make_shared<EquippedUi>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto EquippedUi::ChangeShow() -> void
{
	_is_show = !_is_show;
	_pos = { g_WinCX / 2.f, g_WinCY / 2.f, 0 };
	SoundManager::GetInstance().StopSound(SoundManager::kUi);
	if (_is_show)
	{
		SoundManager::GetInstance().PlaySound(L"System_defaultDialog_open_02.wav", SoundManager::kUi);
	}
	else
	{
		SoundManager::GetInstance().PlaySound(L"System_defaultDialog_close_02.wav", SoundManager::kUi);
	}
}

auto EquippedUi::MoveInventoryFrame() -> HRESULT
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

auto EquippedUi::AddComponents() -> HRESULT
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Equipped"),
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

	for (int i = 0; i < Protocol::kStatType::kStatEnd; ++i)
	{
		_statup_btn[i] = StatupBtn::Create(
			[=]()
			{
				Protocol::GameClientStatUp sendPkt;
				sendPkt.set_type(static_cast<Protocol::kStatType>(i));
				SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			}
		);

	}
	return S_OK;
}

auto EquippedUi::CloseButtonTick(double timeDelta) -> HRESULT
{
	_close_btn_over = false;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>((_pos.x + 220) - 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 125) - 24 * 0.5f),
	   static_cast<LONG>((_pos.x + 220) + 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 125) + 24 * 0.5f)
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

auto EquippedUi::ItemInfoRender() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	const auto baseInventory = objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
	if (auto inventory = std::static_pointer_cast<Inventory>(baseInventory))
	{
		const auto tab = inventory->GetInventoryTab(Protocol::kInventoryEquipped);
		const auto items = tab->AllItem();
		for (auto& item : items)
		{
			if (item.second)
			{
				switch (static_cast<GameContents::kEquipeType>(item.second->GetPosition()))
				{
				case GameContents::kEquipeType::kNull:
					break;
				case GameContents::kEquipeType::kCoat:
				{
					auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - 15) + (g_WinCY >> 1), 0);
					item.second->RenderEquipped(pos, _shader_com);
					item.second->SetIsMouseUp(item.second->SearchMouseOverStateEquipped(pos));
					break;
				}
				case GameContents::kEquipeType::kPants:
				{
					auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - -32) + (g_WinCY >> 1), 0);
					item.second->RenderEquipped(pos, _shader_com);
					item.second->SetIsMouseUp(item.second->SearchMouseOverStateEquipped(pos));
					break;
				}
				case GameContents::kEquipeType::kShoes:
				{

					auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - -76) + (g_WinCY >> 1), 0);
					item.second->RenderEquipped(pos, _shader_com);
					item.second->SetIsMouseUp(item.second->SearchMouseOverStateEquipped(pos));
					break;
				}
				case GameContents::kEquipeType::kWeapon:
				{
					auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - 65) + (g_WinCY >> 1), 0);
					item.second->RenderEquipped(pos, _shader_com);
					item.second->SetIsMouseUp(item.second->SearchMouseOverStateEquipped(pos));
					break;
				}
				default:
					break;;
				}
			}
		}

		for (const auto& item : items)
		{
			if (item.second)
			{
				if (item.second->MouseOverState())
				{
					item.second->GetPopupInfo()->Render();
				}
			}
		}
	}
	return S_OK;
}

auto EquippedUi::CharacterInfoRender() -> HRESULT
{
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(_pos.x - 230),
	   static_cast<LONG>(_pos.y - 150),
	   static_cast<LONG>(_pos.x),
	   static_cast<LONG>(_pos.y)
	};
	auto info = GameLogicQueue::GetInstance()->GetCharacterInfo();
	std::wstring str = fmt::format(L"캐릭터 이름 : {0}\r\n레벨 : {1}", StringUtils::ConvertCtoW(info.name().c_str()), CharacterStat::GetInstance().GetLevel());
	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI, DT_VCENTER | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	return S_OK;
}

auto EquippedUi::CharacterStatRender() -> HRESULT
{
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	RECT		rcUI = {
	   static_cast<LONG>(_pos.x + 120),
	   static_cast<LONG>(_pos.y - 100),
	   static_cast<LONG>(_pos.x + 215),
	   static_cast<LONG>(_pos.y)
	};
	const auto& stat = CharacterStat::GetInstance();
	std::wstring str = fmt::format(L"{0}\r\n\r\n{1}\r\n\r\n{2}\r\n\r\n{3}", stat.GetStr(), stat.GetDex(), stat.GetInt(), stat.GetLuk());
	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI, DT_VCENTER | DT_RIGHT, D3DCOLOR_ARGB(255, 255, 255, 255));

	
	rcUI = {
	   static_cast<LONG>(_pos.x + 200),
	   static_cast<LONG>(_pos.y - 85),
	   static_cast<LONG>(_pos.x + 250),
	   static_cast<LONG>(_pos.y + 250)
	};
	str = fmt::format(L"{0}", stat.GetAp());
	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rcUI, DT_VCENTER | DT_LEFT, D3DCOLOR_ARGB(255, 255, 94, 0));
	return S_OK;
}

auto EquippedUi::UnEqpItem(std::shared_ptr<InventoryTabBtn> equippedTab, std::shared_ptr<Inventory> inventory, GameContents::kEquipeType type) -> void
{
	if (type == GameContents::kEquipeType::kWeapon)
	{
		auto eqpTab = inventory->GetInventoryTab(Protocol::kInventoryEqp);
		auto item = equippedTab->FindItem(static_cast<int32_t>(type));
		if (item)
		{
			const auto basePlayer = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0);
			const auto player = std::static_pointer_cast<Player>(basePlayer);
			if (player)
			{
				equippedTab->RemoveItem(static_cast<int32_t>(type));
				auto freeSlot = eqpTab->GetFreeSlot();
				item->ChangePosition(freeSlot);
				eqpTab->AddItem(item->GetPosition(), item);
				WeaponManager::GetInstance().RemoveWeapon(GameLogicQueue::GetInstance()->GetCharacterInfo().character_id());
				Protocol::GameClientInventoryItemMove sendPkt;
				sendPkt.set_type(Protocol::kInventoryEquipped);
				sendPkt.set_move_type(Protocol::kUnEquipment);
				sendPkt.set_src(static_cast<int32_t>(GameContents::EquipeType(item->GetItemId())));
				sendPkt.set_dst(freeSlot);
				SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			}
		}
	}
	else
	{
		auto eqpTab = inventory->GetInventoryTab(Protocol::kInventoryEqp);
		auto item = equippedTab->FindItem(static_cast<int32_t>(type));
		if (item)
		{
			const auto basePlayer = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0);
			const auto player = std::static_pointer_cast<Player>(basePlayer);
			if (player)
			{
				player->ChangeEqp(type, 0);
				equippedTab->RemoveItem(static_cast<int32_t>(type));
				auto freeSlot = eqpTab->GetFreeSlot();
				item->ChangePosition(freeSlot);
				eqpTab->AddItem(item->GetPosition(), item);
				Protocol::GameClientInventoryItemMove sendPkt;
				sendPkt.set_type(Protocol::kInventoryEquipped);
				sendPkt.set_move_type(Protocol::kUnEquipment);
				sendPkt.set_src(static_cast<int32_t>(GameContents::EquipeType(item->GetItemId())));
				sendPkt.set_dst(freeSlot);
				SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			}
		}
	}
}

auto EquippedUi::ColliderCoat() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - 15) + (g_WinCY >> 1), 0);
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

auto EquippedUi::ColliderPants() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - -32) + (g_WinCY >> 1), 0);
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

auto EquippedUi::ColliderShoes() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - -76) + (g_WinCY >> 1), 0);
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

auto EquippedUi::ColliderWeapon() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;
	auto pos = _float3((_pos.x + 18) + -(g_WinCX >> 1), -(_pos.y - 65) + (g_WinCY >> 1), 0);
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
