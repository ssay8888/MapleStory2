#include "c_pch.h"
#include "berserker_breakingskull_state.h"

#include "src/game_object/player/player.h"

auto BerserkerBreakingSkullState::Enter() -> void
{
	_player->ChangeAnimation(kAnimationType::kBreakingSkull1A);
}

auto BerserkerBreakingSkullState::HandleInput() -> void
{
}

auto BerserkerBreakingSkullState::Tick(const double timeDelta) -> void
{
}

auto BerserkerBreakingSkullState::LateTick(const double timeDelta) -> void
{
	_player->PlayAnimation(timeDelta);
}

auto BerserkerBreakingSkullState::Render() -> void
{
}
