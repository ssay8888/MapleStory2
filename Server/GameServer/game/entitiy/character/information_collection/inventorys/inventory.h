#pragma once
#include <shared_mutex>

#include "protocol/game_enum.pb.h"

class Inventory
{
public:
	explicit Inventory(Protocol::kInventoryType  type);
	~Inventory() = default;

	auto PushItem(int32_t position, int32_t itemId)->bool;
	auto FindItem(int32_t position)->int32_t;
	auto RemoveItem(int32_t position)->bool;
	auto AllItems()->std::vector<std::pair<int32_t, int32_t>>;

private:
	std::shared_mutex _lock;
	std::map<int32_t, int32_t> _items;
	Protocol::kInventoryType _type;
};

