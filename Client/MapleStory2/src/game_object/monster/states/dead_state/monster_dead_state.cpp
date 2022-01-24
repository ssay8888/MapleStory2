#include "c_pch.h"
#include "monster_dead_state.h"

#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/monster/monster.h"
#include "src/managers/character_stat/character_stat.h"
#include "src/utility/game_objects/manager/object_manager.h"

auto MonsterDeadState::Enter(std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterDeadState::HandleInput() -> void
{
}

auto MonsterDeadState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterDeadState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	monster->PlayAnimation(timeDelta);

	auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetMonsterInfo().monster_id());

	auto index = monster->GetStateIndex(Protocol::kDeadA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (monster->GetAnimationTimeAcc() >= endTime)
	{
		if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(CharacterStat::GetInstance().GetMapName()))
		{
			monster->SetDead(true);
			//mapInstance->RemoveMonster(monster->GetMonsterInfo().object_id());
		}
	}
}

auto MonsterDeadState::Render(std::shared_ptr<Monster> monster) -> void
{
}
