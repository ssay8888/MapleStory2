#include "common_pch.h"
#include "item_model.h"

auto ItemModel::PushSlotItem(const std::shared_ptr<Slot>& item)->void
{
	_slots.slot.emplace(item->name, item);
}

auto ItemModel::PushCuttingItem(const std::shared_ptr<Cutting>& item) -> void
{
	_cuttings.push_back(item);
}

auto ItemModel::SetId(const int32_t id) -> void
{
	_id = id;
}

auto ItemModel::GetId() const -> int32_t
{
	return _id;
}

auto ItemModel::SetDesc(const std::string& desc)->void
{
	_desc = desc;
}

auto ItemModel::GetDesc() const -> std::string
{
	return _desc;
}

auto ItemModel::GetSlots() const -> const Slots&
{
	return _slots;
}
