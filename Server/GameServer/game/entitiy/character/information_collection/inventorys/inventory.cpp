#include "game_server_pch.h"
#include "inventory.h"

#include "inventorys.h"
#include "game/entitiy/item/game_item.h"

Inventory::Inventory(const Protocol::kInventoryType type):
	_type(type)
{
}

auto Inventory::PushItem(const int32_t position, const std::shared_ptr<GameItem> itemId) -> bool
{
	std::unique_lock ul(_lock);
	return _items.emplace(position, itemId).second;
}

auto Inventory::FindItem(const int32_t position) -> std::shared_ptr<GameItem>
{
	std::shared_lock sl(_lock);
	const auto iterator = _items.find(position);
	if (iterator != _items.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto Inventory::SwapItem(const int32_t src, const int32_t dst) -> void
{
	std::unique_lock ul(_lock);
	const auto dstItem = _items[dst];
	const auto srcItem = _items[src];

	if (dstItem)
	{
		dstItem->SetPosition(src);
	}
	if (srcItem)
	{
		srcItem->SetPosition(dst);
	}
	_items[dst] = srcItem;
	_items[src] = dstItem;
}

auto Inventory::RemoveItem(const int32_t position) -> bool
{
	std::unique_lock ul(_lock);
	return _items.erase(position);
}

auto Inventory::AllItems() -> std::vector<std::shared_ptr<GameItem>>
{
	std::vector<std::shared_ptr<GameItem>>  items;

	for (auto& item : _items)
	{
		items.push_back(item.second);
	}

	return items;
}
