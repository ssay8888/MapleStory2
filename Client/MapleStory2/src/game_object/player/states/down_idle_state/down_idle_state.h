#pragma once
#include "src/game_object/player/states/character_state.h"

class DownIdleState final : public CharacterState
{
public:
	~DownIdleState() override = default;

	static auto GetInstance()->std::shared_ptr<DownIdleState>
	{
		static auto instance = std::make_shared<DownIdleState>();
		return instance;
	}

	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;
private:
	bool _is_move;

	uint64_t _dead_time;
};

