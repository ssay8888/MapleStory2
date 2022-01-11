#include "c_pch.h"
#include "monster_walk_state.h"

#include "randomizer/randomizer.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto MonsterWalkState::Enter(std::shared_ptr<Monster> monster) -> void
{
	//auto transform = monster->GetTransform();
	//transform->SetMovingState(true);
	//int64_t degree = Randomizer::Rand(static_cast<int64_t>(0), 360);
	//_radian = D3DXToRadian(degree);
	//_is_move = true;
	//ReloadMapObject(monster, true);
	_next_state.set_state(Protocol::kRunA);
}

auto MonsterWalkState::HandleInput() -> void
{
}

auto MonsterWalkState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	const auto transform = monster->GetTransform();
	//transform->BackStraight(timeDelta);

	//auto range = monster->GetReloadRangeAabb();
	//monster->GetMonsterColliderAabb()->UpdateCollider();
	//for (const auto& reload : range)
	//{
	//	reload->UpdateCollider();
	//}
	//
	transform->BackStraight(timeDelta);
	

}

auto MonsterWalkState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	//ReloadMapObject(monster);
	monster->PlayAnimation(timeDelta);
	const auto transform = monster->GetTransform();
	_float3 position = transform->GetState(Transform::kState::kStatePosition);
	_float3	dir = _targetPos - position;
	const float	distance = D3DXVec3Length(&dir);
	D3DXVec3Normalize(&dir, &dir);
	if (distance < 0.01f)
	{
		transform->SetState(Transform::kState::kStatePosition, _targetPos);
		if (_next_state.state() != Protocol::kRunA)
		{
			monster->ChangeAnimation(_next_state.state());
		}
	}
	else if (distance >= 0.2)
	{
		transform->SetState(Transform::kState::kStatePosition, _targetPos);
		if (_next_state.state() != Protocol::kRunA)
		{
			monster->ChangeAnimation(_next_state.state());
		}
	}
	//auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetMonsterInfo().monster_id());

	//auto index = monster->GetStateIndex(Monster::kMonsterState::kIdleA);
	//auto seq = kfm->seqs[index];
	//auto endTime = seq->key[L"end"];
	//if (monster->GetAnimationTimeAcc() >= endTime * 2.f)
	//{
	//	if (Randomizer::IsSuccess(0))
	//	{
	//		monster->ChangeAnimation(Monster::kMonsterState::kBoreA);
	//	}
	//	else
	//	{
	//		monster->ChangeAnimation(Monster::kMonsterState::kIdleA);
	//	}
	//}
}

auto MonsterWalkState::Render(std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterWalkState::GetTargetPos() const -> _float3
{
	return _targetPos;
}

auto MonsterWalkState::SetTargetPos(_float3 pos) -> void
{
	_targetPos = pos;
}

auto MonsterWalkState::GetNextState() const -> Protocol::GameServerMoveMonster
{
	return _next_state;
}

auto MonsterWalkState::SetNextState(Protocol::GameServerMoveMonster state) -> void
{
	_next_state = state;
}
