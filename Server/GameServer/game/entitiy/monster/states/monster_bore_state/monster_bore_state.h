#pragma once
#include "game/entitiy/monster/states/monster_state.h"

class MonsterBoreState : public MonsterState
{
public:
	~MonsterBoreState() override = default;
	auto Enter(std::shared_ptr<GameMonster> monster) -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
};

