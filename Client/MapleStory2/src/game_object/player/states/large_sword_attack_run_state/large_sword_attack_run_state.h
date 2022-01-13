#pragma once
#include "src/game_object/player/states/character_state.h"

class LargeSwordAttackRunState final : public CharacterState
{
public:
	static auto GetInstance()->std::shared_ptr<LargeSwordAttackRunState>
	{
		static auto instance = std::make_shared<LargeSwordAttackRunState>();
		return instance;
	}
	~LargeSwordAttackRunState() override = default;
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool	_is_move = false;
	bool	_is_jump = false;
	_float3 _axis = _float3(0.f, 1.f, 0.f);
};

