#pragma once
#include "src/game_object/player/states/character_state.h"

class MapObject;

class BerserkerBreakingSkullState final : public CharacterState
{
public:
	~BerserkerBreakingSkullState() override = default;

	static auto GetInstance()->std::shared_ptr<BerserkerBreakingSkullState>
	{
		static auto instance = std::make_shared<BerserkerBreakingSkullState>();
		return instance;
	}

public:
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:

};

