#pragma once
#include "protocol/game_enum.pb.h"
class MapObject;
class Monster;

class MonsterState abstract
{
public:
	virtual ~MonsterState() = default;

public:
	virtual auto Enter(std::shared_ptr<Monster> monster)->void = 0;
	virtual auto HandleInput()->void = 0;
	virtual auto Tick(const double timeDelta, std::shared_ptr<Monster> monster)->void = 0;
	virtual auto LateTick(const double timeDelta, std::shared_ptr<Monster> monster)->void = 0;
	virtual auto Render(std::shared_ptr<Monster> monster)->void = 0;


public:
	auto GravityMonster(double timeDelta, std::shared_ptr<Monster> monster)->bool;
	auto BlockUpCheck(std::shared_ptr<Monster> monster)->bool;
	auto StraightCheck(std::shared_ptr<Monster> monster)->bool;
	auto LoadLastTile(std::shared_ptr<Monster> monster)->std::shared_ptr<MapObject>;

protected:
	auto ReloadMapObject(std::shared_ptr<Monster> monster, bool check = false)->void;

protected:
	std::shared_ptr<MapObject>								_last_tile_map_object;
	std::shared_ptr<MapObject>								_last_wall_map_object;
	std::vector<std::shared_ptr<MapObject>>					_map_objects;
};

