#pragma once
#include "game/entitiy/game_entity.h"
#include "protocol/game_enum.pb.h"

class MonsterStat;
class MonsterState;
class Collider;
class Transform;
class SpawnPoint;
class MapInstance;

class GameMonster : public GameEntity
{
public:
	explicit GameMonster(std::shared_ptr<SpawnPoint> spawnPoint, std::shared_ptr<MapInstance> mapInstance);
	virtual ~GameMonster() = default;

	enum kReloadRegenAabb
	{
		kReloadUp,
		kReloadDown,
		kReloadLeft,
		kReloadRight,
		kReloadFront,
		kReloadBack,
		kReloadEnd
	};
public:
	auto Tick(double timeDelta) -> void override;
	auto LateTick(double timeDelta) -> void override;

public:
	static auto Create(const std::shared_ptr<SpawnPoint>& spawnPoint, const std::shared_ptr<MapInstance>& mapInstance)->std::shared_ptr<GameMonster>;


public:
	auto GetTransform() const->std::shared_ptr<Transform>;
	auto GetSpawnPoint() const->std::shared_ptr<SpawnPoint>;
	auto GetMapInstance() const->std::shared_ptr<MapInstance>;
	auto GetMonster()->std::shared_ptr<GameMonster>;
	auto GetStateIndex(Protocol::kMonsterState state)->int32_t;
	auto GetStat()const->std::shared_ptr<MonsterStat>;

	auto GetMonsterColliderAabb()const->std::shared_ptr<Collider>;
	auto GetReloadRangeAabb()const->std::vector<std::shared_ptr<Collider>>;
	auto GetBlockRangeAabb()const->std::shared_ptr<Collider>;

	auto ChangeState(Protocol::kMonsterState state)->void;

private:
	auto NativeContruct()->HRESULT;
	auto AnimationLoad()->HRESULT;

private:
	std::shared_ptr<MapInstance>	_map_instance;
	std::shared_ptr<SpawnPoint>		_spawn_point;
	std::shared_ptr<Transform>		_transform;
	std::shared_ptr<Collider>		_aabb_com = nullptr;
	std::shared_ptr<Collider>											_block_range_aabb_com = nullptr; // 블록가져올 범위
	std::vector<std::shared_ptr<Collider>>								_reload_range_aabb_com;

	std::shared_ptr<MonsterStat>	_monster_stat;


	std::map<int32_t, std::shared_ptr<MonsterState>>					_monster_states;
	std::map<Protocol::kMonsterState, int32_t>							_state_index;
	std::shared_ptr<MonsterState>										_current_monster_state;
};

