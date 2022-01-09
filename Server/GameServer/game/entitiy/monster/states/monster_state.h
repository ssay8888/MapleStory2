#pragma once
class MapXblock;
class GameMonster;

class MonsterState abstract
{
public:
	virtual ~MonsterState() = default;

public:
	virtual auto Enter(std::shared_ptr<GameMonster> monster)->void = 0;
	virtual auto Tick(const double timeDelta, std::shared_ptr<GameMonster> monster)->void = 0;
	virtual auto LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster)->void = 0;

	auto GravityMonster(double timeDelta, std::shared_ptr<GameMonster> monster) -> bool;
	auto BlockUpCheck(std::shared_ptr<GameMonster> monster)->bool;
	auto StraightCheck(std::shared_ptr<GameMonster> monster)->bool;
	auto LoadLastTile(std::shared_ptr<GameMonster> monster)->std::shared_ptr<MapXblock>;

	auto FindTargetCharacter(std::shared_ptr<GameMonster> monster)->std::shared_ptr<GameCharacter>;
	auto CheckTargetCharacterDistance(std::shared_ptr<GameMonster> monster, float distance)->bool;

protected:
	auto ReloadMapObject(std::shared_ptr<GameMonster> monster, bool check = false)->void;

protected:
	bool _first = false;
	double _animation_acc = 0.0;
	std::shared_ptr<MapXblock>								_last_tile_map_object;
	std::shared_ptr<MapXblock>								_last_wall_map_object;
	std::vector<std::shared_ptr<MapXblock>>					_map_objects;
};

