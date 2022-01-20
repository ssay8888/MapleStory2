#pragma once
#include "src/game_object/player/states/character_state.h"

class DownState final : public CharacterState
{
public:
	~DownState() override = default;

	static auto GetInstance()->std::shared_ptr<DownState>
	{
		static auto instance = std::make_shared<DownState>();
		return instance;
	}

	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;
};

