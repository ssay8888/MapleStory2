#pragma once
#include "protocol/game_protocol.pb.h"
#include "src/game_object/monster/states/monster_state.h"

class MonsterRunState : public MonsterState
{
public:
	~MonsterRunState() override = default;
	auto Enter(std::shared_ptr<Monster> monster) -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto Render(std::shared_ptr<Monster> monster) -> void override;

public:
	auto GetTargetPos()const->_float3;
	auto SetTargetPos(_float3 pos)->void;

	auto GetNextState()const->Protocol::GameServerMoveMonster;
	auto SetNextState(Protocol::GameServerMoveMonster state)->void;

private:
	_float3 _targetPos;
	Protocol::GameServerMoveMonster _next_state;
};

