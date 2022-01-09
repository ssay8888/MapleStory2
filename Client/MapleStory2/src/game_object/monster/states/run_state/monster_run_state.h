#pragma once
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
};

