#include "game_server_pch.h"
#include "game_item.h"

GameItem::GameItem(const int32_t itemId, const int32_t position, const Protocol::kInventoryType type):
	_item_id(itemId),
	_position(position),
	_type(type)
{
}

auto GameItem::Tick(double timeDelta) -> void
{
}

auto GameItem::LateTick(double timeDelta) -> void
{
}

auto GameItem::Create(int32_t itemId, int32_t position, Protocol::kInventoryType type) -> std::shared_ptr<GameItem>
{
	auto instance = MakeShared<GameItem>(itemId, position, type);
	
	return instance;
}

auto GameItem::GetItemId() const -> int32_t
{
	return _item_id;
}

auto GameItem::GetPosition() const -> int32_t
{
	return _position;
}

auto GameItem::SetPosition(int32_t position) -> void
{
	_position = position;
}

auto GameItem::GetInventoryType() const -> Protocol::kInventoryType
{
	return _type;
}

auto GameItem::SetInventoryType(Protocol::kInventoryType type) -> void
{
	_type = type;
}

auto GameItem::SetQuantity(int32_t value) -> void
{
	_quantity = value;
}

auto GameItem::GetQuantity() const -> int32_t
{
	return _quantity;
}

auto GameItem::SetStr(int32_t str) -> void
{
	_str = str;
}

auto GameItem::GetStr() const -> int32_t
{
	return _str;
}

auto GameItem::SetDex(int32_t value) -> void
{
	_dex = value;
}

auto GameItem::GetDex() const -> int32_t
{
	return _dex;
}

auto GameItem::SetInt(int32_t value) -> void
{
	_int = value;
}

auto GameItem::GetInt() const -> int32_t
{
	return _int;
}

auto GameItem::SetLuk(int32_t value) -> void
{
	_luk = value;
}

auto GameItem::GetLuk() const -> int32_t
{
	return _luk;
}

auto GameItem::SetWap(int32_t value) -> void
{
	_wap = value;
}

auto GameItem::GetWap() const -> int32_t
{
	return _wap;
}

