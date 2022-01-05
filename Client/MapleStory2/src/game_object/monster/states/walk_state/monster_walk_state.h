#pragma once
#include "src/game_object/monster/states/monster_state.h"

class MonsterWalkState : public MonsterState
{
public:
	~MonsterWalkState() override = default;
	auto Enter(std::shared_ptr<Monster> monster) -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto Render(std::shared_ptr<Monster> monster) -> void override;

private:
	bool	_is_move = false;
	_float3 _axis = _float3(0.f, 1.f, 0.f);
	float _radian;
};

