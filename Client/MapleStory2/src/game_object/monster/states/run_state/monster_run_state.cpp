#include "c_pch.h"
#include "monster_run_state.h"

#include "src/game_object/monster/monster.h"
#include "src/utility/components/transform/transform.h"

auto MonsterRunState::Enter(std::shared_ptr<Monster> monster) -> void
{/*
	if (monster->GetMonsterInfo().monster_id() == 43000101)
	{
		_next_state.set_state(Protocol::kRunA);
	}
	else
	{
		_next_state.set_state(Protocol::kRunA);
	}*/

	_next_state.set_state(Protocol::kRunA);
}

auto MonsterRunState::HandleInput() -> void
{
}

auto MonsterRunState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monster->GetMonsterInfo().monster_id());

	if (monsterInfo)
	{
		const auto transform = monster->GetTransform();
		transform->BackStraight(timeDelta, monsterInfo->model.run_speed);
	}
}

auto MonsterRunState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	monster->PlayAnimation(timeDelta);
	const auto transform = monster->GetTransform();
	_float3 position = transform->GetState(Transform::kState::kStatePosition);
	_float3	dir = _targetPos - position;
	const float	distance = D3DXVec3Length(&dir);
	D3DXVec3Normalize(&dir, &dir);
	if (distance < 0.05f)
	{
		transform->SetState(Transform::kState::kStatePosition, _targetPos);
		monster->ChangeAnimation(_next_state.state());
	}
	else if (distance >= 0.35f)
	{
		transform->SetState(Transform::kState::kStatePosition, _targetPos);
		monster->ChangeAnimation(_next_state.state());
	}
}

auto MonsterRunState::Render(std::shared_ptr<Monster> monster) -> void
{
}

auto MonsterRunState::GetTargetPos() const -> _float3
{
	return _targetPos;
}

auto MonsterRunState::SetTargetPos(const _float3 pos) -> void
{
	_targetPos = pos;
}

auto MonsterRunState::GetNextState() const -> Protocol::GameServerMoveMonster
{
	return _next_state;
}

auto MonsterRunState::SetNextState(Protocol::GameServerMoveMonster state) -> void
{
	_next_state = state;
}
