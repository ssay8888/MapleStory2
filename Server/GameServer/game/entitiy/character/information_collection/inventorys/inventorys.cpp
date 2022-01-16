#include "game_server_pch.h"
#include "inventorys.h"

#include <pugixml.hpp>
#include <sstream>

#include "inventory.h"
#include "game/entitiy/item/game_item.h"

Inventorys::Inventorys(const int64_t characterId) :
	BaseInfo(characterId)
{
}

auto Inventorys::PushItem(const Protocol::kInventoryType type, const int32_t position, const std::shared_ptr<GameItem> itemId) -> bool
{
	auto iterator = _inventory.find(type);
	if (iterator == _inventory.end())
	{
		return false;
	}
	return iterator->second->PushItem(position, itemId);
}

auto Inventorys::FindItem(const Protocol::kInventoryType type, const int32_t position) -> std::shared_ptr<GameItem>
{
	const auto iterator = _inventory.find(type);
	if (iterator == _inventory.end())
	{
		return nullptr;
	}
	return iterator->second->FindItem(position);
}

auto Inventorys::SwapItem(Protocol::kInventoryType type, int32_t src, int32_t dst) -> void
{
	const auto iterator = _inventory.find(type);
	if (iterator == _inventory.end())
	{
		return;
	}
	iterator->second->SwapItem(src, dst);
}

auto Inventorys::RemoveItem(const Protocol::kInventoryType type, const int32_t position) -> bool
{
	const auto iterator = _inventory.find(type);
	if (iterator == _inventory.end())
	{
		return false;
	}
	return iterator->second->RemoveItem(position);
}

auto Inventorys::AllItems() -> std::vector<std::shared_ptr<GameItem>>
{
	std::vector<std::shared_ptr<GameItem>> allItems;
	for (const auto& inventory : _inventory)
	{
		auto items = inventory.second->AllItems();
		allItems.insert(allItems.end(), items.begin(), items.end());
	}
	return allItems;
}

auto Inventorys::AllItems(Protocol::kInventoryType type) -> std::vector<std::shared_ptr<GameItem>>
{
	std::vector<std::shared_ptr<GameItem>> allItems;
	auto iterator = _inventory.find(Protocol::kInventoryType::kInventoryEquipped);
	if (iterator != _inventory.end())
	{
		auto items = iterator->second->AllItems();
		allItems.insert(allItems.end(), items.begin(), items.end());
	}
	return allItems;
}

auto Inventorys::FindInventory(Protocol::kInventoryType type) -> std::shared_ptr<Inventory>
{
	return _inventory[type];
}

auto Inventorys::ItemListToXml() const -> std::wstring
{
	using namespace pugi;
	xml_document doc;
	xml_node root = doc.append_child("ms2");

	auto items = root.append_child("items");
	for (const auto& [type, inventory] : _inventory)
	{
		const auto allItems = inventory->AllItems();
		if (!allItems.empty())
		{
			for (const auto& itemData : allItems)
			{
				if (itemData)
				{
					auto item = items.append_child("item");
					item.append_attribute("type").set_value(itemData->GetInventoryType());
					item.append_attribute("itemid").set_value(itemData->GetItemId());
					item.append_attribute("position").set_value(itemData->GetPosition());
				}
			}
		}
	}
	std::wstringstream xml;
	doc.save(xml);
	return xml.str();
}

auto Inventorys::Create(const int64_t characterId) -> std::shared_ptr<Inventorys>
{
	auto instance = MakeShared<Inventorys>(characterId);

	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}

	return instance;
}

auto Inventorys::NativeConstruct() -> HRESULT
{
	constexpr auto maxCount = static_cast<int32_t>(Protocol::kInventoryType::kInventoryEnd);
	for (int i = 0; i < maxCount; ++i)
	{
		const auto invenType = static_cast<Protocol::kInventoryType>(i);
		const auto inventory = std::make_shared<Inventory>(invenType);
		_inventory.emplace(invenType, inventory);
	}
	return S_OK;
}
