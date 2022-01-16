#pragma once
#include "game/entitiy/game_entity.h"
#include "protocol/game_enum.pb.h"

class GameItem : public GameEntity
{
public:
	GameItem(int32_t itemId, int32_t position, Protocol::kInventoryType type);
	~GameItem() override = default;
	auto Tick(double timeDelta) -> void override;
	auto LateTick(double timeDelta) -> void override;

public:
	static auto Create(int32_t itemId, int32_t position, Protocol::kInventoryType type)->std::shared_ptr<GameItem>;

public:
	auto GetItemId()const->int32_t;

	auto GetPosition()const->int32_t;
	auto SetPosition(int32_t position)->void;

	auto GetInventoryType()const->Protocol::kInventoryType;
	auto SetInventoryType(Protocol::kInventoryType type)->void;

	auto SetQuantity(int32_t value) -> void;
	auto GetQuantity()const->int32_t;

	auto SetStr(int32_t str)->void;
	auto GetStr()const->int32_t;

	auto SetDex(int32_t value)->void;
	auto GetDex()const->int32_t;

	auto SetInt(int32_t value)->void;
	auto GetInt()const->int32_t;

	auto SetLuk(int32_t value)->void;
	auto GetLuk()const->int32_t;

	auto SetWap(int32_t value)->void;
	auto GetWap()const->int32_t;

private:
	int32_t _item_id;
	int32_t _position;
	Protocol::kInventoryType _type;

	int32_t _quantity;
	int32_t _str;
	int32_t _dex;
	int32_t _int;
	int32_t _luk;
	int32_t _wap;
};

