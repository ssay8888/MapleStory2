#pragma once
#include "src/game_object/player/states/character_state.h"

class IdleState final :
    public CharacterState
{
public:
	~IdleState() override = default;

	static auto GetInstance()->std::shared_ptr<IdleState>
	{
		static auto instance = std::make_shared<IdleState>();
		return instance;
	}

public:
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool	_is_move = false;
	bool	_is_attack = false;
	bool	_is_jump = false;
};

