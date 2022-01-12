#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/game_object/monster/states/monster_state.h"

class Collider;

class MonsterAttackAState : public MonsterState
{
public:
	explicit MonsterAttackAState(Protocol::kMonsterState state);
	~MonsterAttackAState() override = default;
	auto Enter(std::shared_ptr<Monster> monster) -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto Render(std::shared_ptr<Monster> monster) -> void override;

public:
	auto GetTargetPos()const->_float3;
	auto SetTargetPos(_float3 pos)->void;

private:
	const Protocol::kMonsterState _state;
	std::vector<std::shared_ptr<DataReaderManager::Motion>> _use_motions;
	std::shared_ptr<DataReaderManager::Motion> _use_skill;
	std::vector<std::pair<int32_t, int32_t>> _skills;
	_float3 _targetPos;

	std::shared_ptr<Collider> _aabb_com;

	float _min_distance;
	bool _is_player_attack;
};

