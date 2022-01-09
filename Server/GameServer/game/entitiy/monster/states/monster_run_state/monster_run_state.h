#pragma once
#include "game/entitiy/monster/states/monster_state.h"

class MonsterRunState : public MonsterState
{
public:
	~MonsterRunState() override = default;
	auto Enter(std::shared_ptr<GameMonster> monster) -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
};

