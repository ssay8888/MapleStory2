#pragma once
#include "src/game_object/player/states/character_state.h"

class MapObject;

class MoveState : public CharacterState
{
public:
	~MoveState() override = default;

	static auto GetInstance()->std::shared_ptr<MoveState>
	{
		static auto instance = std::make_shared<MoveState>();
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
	bool	_is_jump = false;
	_float3 _axis = _float3(0.f, 1.f, 0.f);
	float	_radian = 0;
};

