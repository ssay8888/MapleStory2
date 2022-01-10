#include "c_pch.h"
#include "monster_idle_state.h"

#include "randomizer/randomizer.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"

auto MonsterIdleState::Enter(std::shared_ptr<Monster> monster) -> void
{
	//if (!_first)
	//{
	//	int count = 0;
	//	while (GravityMonster(0.0001, monster))
	//	{
	//		ReloadMapObject(monster);
	//		auto range = monster->GetReloadRangeAabb();
	//		monster->GetMonsterColliderObb()->UpdateCollider();
	//		monster->GetMonsterColliderAabb()->UpdateCollider();
	//		for (const auto& reload : range)
	//		{
	//			reload->UpdateCollider();
	//		}
	//	}
	//	_first = true;
	//}
}

auto MonsterIdleState::HandleInput() -> void
{
}

auto MonsterIdleState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	//auto range = monster->GetReloadRangeAabb();
	//monster->GetMonsterColliderObb()->UpdateCollider();
	//monster->GetMonsterColliderAabb()->UpdateCollider();
	//for (const auto& reload : range)
	//{
	//	reload->UpdateCollider();
	//}
}

auto MonsterIdleState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	monster->PlayAnimation(timeDelta);
	//auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetMonsterInfo().monster_id());

	//auto index = monster->GetStateIndex(Monster::kMonsterState::kIdleA);
	//auto seq = kfm->seqs[index];
	//auto endTime = seq->key[L"end"];
	//if (monster->GetAnimationTimeAcc() >= endTime * 3.f)
	//{
	//	if (Randomizer::IsSuccess(50))
	//	{
	//		monster->ChangeAnimation(Monster::kMonsterState::kBoreA);
	//	}
	//	else 
	//	{
	//		monster->ChangeAnimation(Monster::kMonsterState::kWalkA);
	//	}
	//}
}

auto MonsterIdleState::Render(std::shared_ptr<Monster> monster) -> void
{
}
