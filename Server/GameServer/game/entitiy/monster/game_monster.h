#pragma once
#include "game/entitiy/game_entity.h"

class Collider;
class Transform;
class SpawnPoint;

class GameMonster : public GameEntity
{
public:
	explicit GameMonster(const std::shared_ptr<SpawnPoint>& spawnPoint);
	virtual ~GameMonster() = default;

public:
	static auto Create(const std::shared_ptr<SpawnPoint>& spawnPoint)->std::shared_ptr<GameMonster>;

public:
	auto GetObjectId() const ->int64_t;
	auto GetTransform() const->std::shared_ptr<Transform>;

private:
	auto NativeContruct()->HRESULT;

private:
	int64_t _object_id;
	std::shared_ptr<SpawnPoint>		_spawn_point;
	std::shared_ptr<Transform>		_transform;
	std::shared_ptr<Collider>		_aabb_com = nullptr;

};

