#include "c_pch.h"
#include "monster_bore_state.h"

#include "src/game_object/monster/monster.h"

auto MonsterBoreState::Enter(std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterBoreState::HandleInput() -> void
{
}

auto MonsterBoreState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterBoreState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	auto kfm = DataReaderManager::GetInstance().FindAnyKey(monster->GetMonsterInfo().monster_id());

	auto index = monster->GetStateIndex(Monster::kMonsterState::kBoreA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	monster->PlayAnimation(timeDelta);
	if (monster->GetAnimationTimeAcc() >= endTime)
	{
		monster->ChangeAnimation(Monster::kMonsterState::kIdleA);
	}
}

auto MonsterBoreState::Render(std::shared_ptr<Monster> monster) -> void
{
}
