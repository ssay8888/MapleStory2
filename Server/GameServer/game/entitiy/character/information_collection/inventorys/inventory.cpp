#include "game_server_pch.h"
#include "inventory.h"

#include "inventorys.h"

Inventory::Inventory(const Protocol::kInventoryType type):
	_type(type)
{
}

auto Inventory::PushItem(const int32_t position, const int32_t itemId) -> bool
{
	std::unique_lock ul(_lock);
	return _items.emplace(position, itemId).second;
}

auto Inventory::FindItem(const int32_t position) -> int32_t
{
	std::shared_lock sl(_lock);
	const auto iterator = _items.find(position);
	if (iterator != _items.end())
	{
		return iterator->second;
	}
	return -1;
}

auto Inventory::RemoveItem(const int32_t position) -> bool
{
	std::unique_lock ul(_lock);
	return _items.erase(position);
}

auto Inventory::AllItems() -> std::vector<std::pair<int32_t, int32_t>>
{
	std::vector<std::pair<int32_t, int32_t>>  items;

	for (auto& item : _items)
	{
		items.push_back(std::make_pair(item.first, item.second));
	}

	return items;
}
