#include "c_pch.h"
#include "monster_run_state.h"

#include "src/game_object/monster/monster.h"

auto MonsterRunState::Enter(std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterRunState::HandleInput() -> void
{
}

auto MonsterRunState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterRunState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	monster->PlayAnimation(timeDelta);
}

auto MonsterRunState::Render(std::shared_ptr<Monster> monster) -> void
{
}
