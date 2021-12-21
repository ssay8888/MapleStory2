#include "pch.h"
#include "equipped.h"

auto Equipped::FindItem(const GameContents::kEquipeType type) -> int32_t
{
	auto item = _eqp_list.find(type);
	if (item != _eqp_list.end())
	{
		return item->second;
	}
	return 0;
}

auto Equipped::PushItem(int32_t itemId) -> void
{
	_eqp_list.emplace(GameContents::EquipeType(itemId), itemId);
}

auto Equipped::GetAllItem() const -> std::vector<int32_t>
{
	std::vector<int32_t> items;

	for (const auto& itemPair : _eqp_list)
	{
		items.push_back(itemPair.second);
	}
	return items;
}
