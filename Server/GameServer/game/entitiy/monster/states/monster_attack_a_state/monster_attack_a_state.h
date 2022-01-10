#pragma once
#include "game/entitiy/monster/states/monster_state.h"
#include "protocol/game_enum.pb.h"

class MonsterAttackAState : public MonsterState
{
public:
	explicit MonsterAttackAState(Protocol::kMonsterState state);
	~MonsterAttackAState() override = default;
	auto Enter(std::shared_ptr<GameMonster> monster) -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;

private:
	float wait_skill_delay = 0;
	Protocol::kMonsterState _state;
};

