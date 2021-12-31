#pragma once
#include "src/game_object/player/states/character_state.h"

class JumpState : public CharacterState
{
public:
	~JumpState() override = default;

	static auto GetInstance()->std::shared_ptr<JumpState>
	{
		static auto instance = std::make_shared<JumpState>();
		return instance;
	}

public:
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool _is_jump_up = false;
	bool _is_jump_down = false;

	bool	_is_move = false;

	const float _jump_up_time = 0.31f;
	const float _jump_power = 3.2f;
};

