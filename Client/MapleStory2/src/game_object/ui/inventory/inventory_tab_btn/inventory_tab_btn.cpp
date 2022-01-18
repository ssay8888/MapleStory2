#include "c_pch.h"
#include "inventory_tab_btn.h"

#include "common_defines.h"
#include "protocol/game_protocol.pb.h"
#include "src/game_object/equipped/equipped.h"
#include "src/game_object/item/Item.h"
#include "src/game_object/player/player.h"
#include "src/game_object/ui/equipped_ui/equipped_ui.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/ket_set_ui/key_set_manager.h"
#include "src/game_object/ui/ket_set_ui/key_set_ui.h"

InventoryTabBtn::InventoryTabBtn(Protocol::kInventoryType type, _float3 pos) :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_type(type),
	_pos(pos)
{
}

auto InventoryTabBtn::NativeConstructPrototype() -> HRESULT
{
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	if (FAILED(AddComponents()))
	{
		return S_OK;
	}
	return GameObject::NativeConstructPrototype();
}

auto InventoryTabBtn::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto InventoryTabBtn::Tick(const double timeDelta) -> HRESULT
{
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_T))
	//{
	//	_original_pos.x += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_Y))
	//{
	//	_original_pos.x -= 1;
	//}

	//if (InputDevice::GetInstance().GetKeyPressing(DIK_G))
	//{
	//	_original_pos.y += 1;
	//}
	//if (InputDevice::GetInstance().GetKeyPressing(DIK_H))
	//{
	//	_original_pos.y -= 1;
	//}

	return GameObject::Tick(timeDelta);
}

auto InventoryTabBtn::Tick(const _float3& pos, double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();

	std::shared_ptr<Item> itemObject = nullptr;
	auto lDown = _input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton);
	auto lUp = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton);
	auto rDown = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kRightButton);
	for (const auto [position, item] : _items)
	{
		if (item)
		{
			if (_is_click)
			{
				if (rDown)
				{
					if (_type == Protocol::kInventoryEqp)
					{
						if (item->SearchMouseOverState(pos))
						{
							auto& objectManager = ObjectManager::GetInstance();
							auto baseCharacter = objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0);
							auto player = std::static_pointer_cast<Player>(baseCharacter);
							auto baseInventory = objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
							auto inventory = std::static_pointer_cast<Inventory>(baseInventory);
							if (inventory)
							{
								const auto equippedTab = inventory->GetInventoryTab(Protocol::kInventoryEquipped);
								int32_t equippedPosition = static_cast<int32_t>(GameContents::EquipeType(item->GetItemId()));
								int32_t oldPosition = position;
								auto equippedItem = equippedTab->FindItem(equippedPosition);
								equippedTab->AddItem(equippedPosition, item);
								RemoveItem(oldPosition);
								item->ChangePosition(equippedPosition);
								if (equippedItem)
								{
									_items[oldPosition] = equippedItem;
									equippedItem->ChangePosition(oldPosition);
								}
								player->ChangeEqp(GameContents::EquipeType(item->GetItemId()), item->GetItemId());
								Protocol::GameClientInventoryItemMove sendPkt;
								sendPkt.set_type(_type);
								sendPkt.set_move_type(Protocol::kEquipment);
								sendPkt.set_src(oldPosition);
								sendPkt.set_dst(equippedPosition);
								SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
							}
						}
					}
				}
				else
				{
					if (item->SelectItem(pos, lDown, lUp))
					{
						itemObject = item;
						itemObject->SetSelectItem(true);
					}
					if (item->UnSelectItem(pos, lDown, lUp))
					{
						item->SetSelectItem(false);
						const auto resultPosition = item->FindPosition(pos);
						if (resultPosition >= 0)
						{
							const auto oldPosition = item->GetPosition();
							auto findItem = _items.find(resultPosition);
							if (findItem != _items.end())
							{
								if (findItem->second)
								{
									findItem->second->ChangePosition(oldPosition);
								}
								_items[oldPosition] = findItem->second;
								_items[resultPosition] = item;
							}
							else
							{
								_items[oldPosition] = nullptr;
								_items[resultPosition] = item;
							}
							item->ChangePosition(resultPosition);
							Protocol::GameClientInventoryItemMove sendPkt;
							sendPkt.set_type(_type);
							sendPkt.set_move_type(Protocol::kMove);
							sendPkt.set_src(oldPosition);
							sendPkt.set_dst(resultPosition);
							SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
						}
						else
						{
							item->ResetPosition();
							if (_type == Protocol::kInventoryEtc)
							{
								auto baseKeyManager = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Keymanager", 0);
								auto keyManager = std::static_pointer_cast<KeySetManager>(baseKeyManager);
								if (keyManager)
								{
									auto index = keyManager->IsCollisionKeySet();
									if (index != -1)
									{
										const auto keySet = keyManager->GetKeySet(index);
										keySet->SetItem(item);
										Protocol::GameClientKeySet sendPkt;
										sendPkt.set_key_value(index);
										sendPkt.set_type(Protocol::kItem);
										sendPkt.set_value(item->GetPosition());
										SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
									}
								}
							}
						}
					}
				}
			}
			item->SetIsMouseUp(item->SearchMouseOverState(pos));
		}
	}

	for (const auto& [key, item] : _items)
	{
		if (item)
		{
			item->Tick(pos, timeDelta);
			item->GetPopupInfo()->Tick(timeDelta);
		}
	}
	return S_OK;
}

auto InventoryTabBtn::LateTick(const double timeDelta) -> HRESULT
{
	for (const auto& item : _items)
	{
		if (item.second)
		{
			item.second->LateTick(timeDelta);
		}
	}
	return GameObject::LateTick(timeDelta);
}

auto InventoryTabBtn::Render() -> HRESULT
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

	if (_is_click)
	{
		ItemListRender(pos, shader);
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

auto InventoryTabBtn::ItemListRender(const _float3& pos, std::shared_ptr<Shader> shader) -> HRESULT
{
	for (auto item : _items)
	{
		if (item.second)
		{
			item.second->Render(pos, shader);
		}
	}

	for (auto item : _items)
	{
		if (item.second)
		{
			if (item.second->MouseOverState())
			{
				item.second->GetPopupInfo()->Render();
			}
		}
	}
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

auto InventoryTabBtn::SelectBtn(uint8_t lButton) -> bool
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
		if (lButton)
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

auto InventoryTabBtn::IsSelect() const -> bool
{
	return _is_click;
}

auto InventoryTabBtn::AllItem() const -> std::map<int32_t, std::shared_ptr<Item>>
{
	return _items;
}

auto InventoryTabBtn::AddItem(int32_t position, std::shared_ptr<Item> item) -> void
{
	_items[position] = item;
}

auto InventoryTabBtn::FindItem(const int32_t position) -> std::shared_ptr<Item>
{
	const auto iterator = _items.find(position);
	if (iterator != _items.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto InventoryTabBtn::RemoveItem(int32_t position) -> void
{
	_items[position] = nullptr;
}

auto InventoryTabBtn::GetFreeSlot() const -> int32_t
{
	for (int32_t i = 0; i < kInventoryConstant::kInventoryMaxSlot; ++i)
	{
		auto iterator = _items.find(i);
		if (iterator == _items.end() || iterator->second == nullptr)
		{
			return i;
		}
	}
	return -1;
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
