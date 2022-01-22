#pragma once
#include "src/game_object/player/states/character_state.h"

class LadderState : public CharacterState
{
public:
	~LadderState() override = default;

	static auto GetInstance()->std::shared_ptr<LadderState>
	{
		static auto instance = std::make_shared<LadderState>();
		return instance;
	}

public:
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool _is_left = false;
	bool _is_right = false;
	bool _is_up = false;
	bool _is_down = false;
	bool _is_upper_left = false;
	bool _is_upper_right = false;
	bool _is_bottom_left = false;
	bool _is_bottom_right = false;
	bool _is_jump = false;
};

