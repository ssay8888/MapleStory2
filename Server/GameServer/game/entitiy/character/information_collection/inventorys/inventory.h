#pragma once
#include <shared_mutex>

#include "protocol/game_enum.pb.h"

class GameItem;

class Inventory
{
public:
	explicit Inventory(Protocol::kInventoryType type);
	~Inventory() = default;

	auto PushItem(int32_t position, std::shared_ptr<GameItem> itemId)->bool;
	auto FindItem(int32_t position)->std::shared_ptr<GameItem>;
	auto SwapItem(int32_t src, int32_t dst)->void;
	auto RemoveItem(int32_t position)->bool;
	auto AllItems()->std::vector<std::shared_ptr<GameItem>>;

private:
	std::shared_mutex _lock;
	std::map<int32_t, std::shared_ptr<GameItem>> _items;
	Protocol::kInventoryType _type;
};

