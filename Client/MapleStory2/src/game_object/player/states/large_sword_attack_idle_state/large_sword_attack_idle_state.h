#pragma once
#include "src/game_object/player/states/character_state.h"

class LargeSwordAttackIdleState : public CharacterState
{
public:
	~LargeSwordAttackIdleState() override = default;
	static auto GetInstance()->std::shared_ptr<LargeSwordAttackIdleState>
	{
		static auto instance = std::make_shared<LargeSwordAttackIdleState>();
		return instance;
	}

	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool _is_move = false;
	bool _is_attack = false;
	bool _is_jump = false;
};

