#pragma once

class MapObject;
class Player;

class CharacterState abstract
{
public:
	virtual ~CharacterState() = default;

public:
	virtual auto Enter()->void = 0;
	virtual auto HandleInput()->void = 0;
	virtual auto Tick(const double timeDelta)->void = 0;
	virtual auto LateTick(const double timeDelta)->void = 0;
	virtual auto Render()->void = 0;

public:
	auto GravityPlayer(double timeDelta)->bool;
	auto GravityPlayerSendMessage(kAnimationType type) const->void;
	auto StraightCheck()->bool;

public:
	auto SetPushKey(int32_t key)->void;

protected:
	auto KeySetHandleInput() const->std::pair<int32_t, std::shared_ptr<CharacterState>>;

protected:
	auto ReloadMapObject()->void;

public:
	auto SetPlayer(const std::shared_ptr<Player> player)->void
	{
		_player = player;
	}
	
protected:
	int32_t _key;
	std::shared_ptr<Player>									_player;
	std::shared_ptr<MapObject>								_last_tile_map_object;
	std::shared_ptr<MapObject>								_last_wall_map_object;
	static std::vector<std::shared_ptr<MapObject>>			_map_objects;
};
