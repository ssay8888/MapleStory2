#pragma once
#include "game/entitiy/monster/states/monster_state.h"

class MonsterWalkState : public MonsterState
{
public:
	~MonsterWalkState() override = default;
	auto Enter(std::shared_ptr<GameMonster> monster) -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void override;

private:
	bool	_is_move = false;
	float	_radian;
	_float3 _axis = _float3(0.f, 1.f, 0.f);
};

