#include "game_server_pch.h"
#include "monster_idle_state.h"

#include "data_reader/data_reader_manager.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/map/map_instance.h"
#include "protocol/game_enum.pb.h"
#include "randomizer/randomizer.h"
#include "src/utility/components/transform/transform.h"

auto MonsterIdleState::Enter(std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc = 0.0;
	if (!_first)
	{
		int count = 0;
		while (GravityMonster(0.0001, monster))
		{
			ReloadMapObject(monster);
			auto range = monster->GetReloadRangeAabb();
			monster->GetMonsterColliderAabb()->UpdateCollider();
			for (const auto& reload : range)
			{
				reload->UpdateCollider();
			}
		}
		_first = true;
	}
	Protocol::GameServerMoveMonster sendPkt;
	sendPkt.set_object_id(monster->GetObjectId());
	sendPkt.set_state(Protocol::kIdleA);

	auto sendRight = sendPkt.mutable_right();
	auto right = monster->GetTransform()->GetState(Transform::kState::kStateRight);
	sendRight->set_x(right.x);
	sendRight->set_y(right.y);
	sendRight->set_z(right.z);

	auto sendUp = sendPkt.mutable_up();
	auto up = monster->GetTransform()->GetState(Transform::kState::kStateUp);
	sendUp->set_x(up.x);
	sendUp->set_y(up.y);
	sendUp->set_z(up.z);


	auto sendLook = sendPkt.mutable_look();
	auto look = monster->GetTransform()->GetState(Transform::kState::kStateLook);
	sendLook->set_x(look.x);
	sendLook->set_y(look.y);
	sendLook->set_z(look.z);


	auto sendPos = sendPkt.mutable_position();
	auto pos = monster->GetTransform()->GetState(Transform::kState::kStatePosition);
	sendPos->set_x(pos.x);
	sendPos->set_y(pos.y);
	sendPos->set_z(pos.z);


	sendRight = sendPkt.mutable_cright();
	right = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateRight);
	sendRight->set_x(right.x);
	sendRight->set_y(right.y);
	sendRight->set_z(right.z);

	sendUp = sendPkt.mutable_cup();
	up = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateUp);
	sendUp->set_x(up.x);
	sendUp->set_y(up.y);
	sendUp->set_z(up.z);


	sendLook = sendPkt.mutable_clook();
	look = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateLook);
	sendLook->set_x(look.x);
	sendLook->set_y(look.y);
	sendLook->set_z(look.z);


	sendPos = sendPkt.mutable_cposition();
	pos = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStatePosition);
	sendPos->set_x(pos.x);
	sendPos->set_y(pos.y);
	sendPos->set_z(pos.z);

	monster->GetMapInstance()->BroadCastMessage(sendPkt, nullptr);
}

auto MonsterIdleState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc += timeDelta;
}

auto MonsterIdleState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	const auto kfm = DataReaderManager::GetInstance().FindAnyKey(monster->GetSpawnPoint()->GetSpawnNpcId());

	auto index = monster->GetStateIndex(Protocol::kMonsterState::kIdleA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (_animation_acc >= endTime * 3.f)
	{
		if (Randomizer::IsSuccess(50))
		{
			monster->ChangeState(Protocol::kMonsterState::kWalkA);
		}
		else 
		{
			//monster->ChangeState(Protocol::kMonsterState::kBoreA);
		}
	}
}
