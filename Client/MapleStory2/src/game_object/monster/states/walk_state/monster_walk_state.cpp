#include "c_pch.h"
#include "monster_walk_state.h"

#include "randomizer/randomizer.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto MonsterWalkState::Enter(std::shared_ptr<Monster> monster) -> void
{
	int64_t degree = Randomizer::Rand(static_cast<int64_t>(0), 360);
	_radian = D3DXToRadian(degree);
	_is_move = true;
	ReloadMapObject(monster, true);
}

auto MonsterWalkState::HandleInput() -> void
{
}

auto MonsterWalkState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	auto transform = monster->GetTransform();
	if (_is_move)
	{
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), _radian);
		transform->BackStraight(timeDelta);

		monster->GetMonsterColliderAabb()->UpdateCollider();
		auto range = monster->GetReloadRangeAabb();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}
		_last_tile_map_object = LoadLastTile(monster);
		if (StraightCheck(monster) || !BlockUpCheck(monster))
		{
			transform->BackStraight(-timeDelta);
		}

		monster->GetMonsterColliderObb()->UpdateCollider();
		monster->GetMonsterColliderAabb()->UpdateCollider();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}

	}
}

auto MonsterWalkState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	ReloadMapObject(monster);
	monster->PlayAnimation(timeDelta);
	auto kfm = DataReaderManager::GetInstance().FindAnyKey(monster->GetMonsterInfo().monster_id());

	auto index = monster->GetStateIndex(Monster::kMonsterState::kIdleA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (monster->GetAnimationTimeAcc() >= endTime * 2.f)
	{
		if (Randomizer::IsSuccess(0))
		{
			monster->ChangeAnimation(Monster::kMonsterState::kBoreA);
		}
		else
		{
			monster->ChangeAnimation(Monster::kMonsterState::kIdleA);
		}
	}
}

auto MonsterWalkState::Render(std::shared_ptr<Monster> monster) -> void
{
}
