#include "game_server_pch.h"
#include "monster_bore_state.h"

#include "data_reader/data_reader_manager.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/map/map_instance.h"
#include "protocol/game_enum.pb.h"
#include "protocol/game_protocol.pb.h"
#include "randomizer/randomizer.h"
#include "src/utility/components/transform/transform.h"

auto MonsterBoreState::Enter(std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc = 0.f;
	monster->ChangeTargetCharacter(nullptr);
	Protocol::GameServerMoveMonster sendPkt;
	sendPkt.set_object_id(monster->GetObjectId());
	std::cout << "bore " << monster->GetObjectId() << std::endl;
	sendPkt.set_state(Protocol::kBoreA);

	const auto sendRight = sendPkt.mutable_right();
	const auto right = monster->GetTransform()->GetState(Transform::kState::kStateRight);
	sendRight->set_x(right.x);
	sendRight->set_y(right.y);
	sendRight->set_z(right.z);

	const auto sendUp = sendPkt.mutable_up();
	const auto up = monster->GetTransform()->GetState(Transform::kState::kStateUp);
	sendUp->set_x(up.x);
	sendUp->set_y(up.y);
	sendUp->set_z(up.z);


	const auto sendLook = sendPkt.mutable_look();
	const auto look = monster->GetTransform()->GetState(Transform::kState::kStateLook);
	sendLook->set_x(look.x);
	sendLook->set_y(look.y);
	sendLook->set_z(look.z);


	const auto sendPos = sendPkt.mutable_position();
	const auto pos = monster->GetTransform()->GetState(Transform::kState::kStatePosition);
	sendPos->set_x(pos.x);
	sendPos->set_y(pos.y);
	sendPos->set_z(pos.z);

	monster->GetMapInstance()->BroadCastMessage(sendPkt, nullptr);
}

auto MonsterBoreState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc += timeDelta;

	if (monster->GetTargetCharacter() != nullptr) // 이미 타겟이 있다면
	{
		if (!CheckTargetCharacterDistance(monster, 2.0f)) //거리가 멀어졌다면 더이상 쫒아가지않아..
		{
			monster->ChangeTargetCharacter(nullptr);
		}
	}

	auto character = FindTargetCharacter(monster);
	if (character != nullptr)
	{
		monster->ChangeTargetCharacter(character);
	}
}

auto MonsterBoreState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	if (monster->GetTargetCharacter() != nullptr)
	{
		monster->ChangeState(Protocol::kMonsterState::kRunA);
		return;
	}

	const auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetSpawnPoint()->GetSpawnNpcId());

	auto index = monster->GetStateIndex(Protocol::kMonsterState::kBoreA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (_animation_acc >= endTime)
	{
		if (Randomizer::IsSuccess(50))
		{
			monster->ChangeState(Protocol::kMonsterState::kWalkA);
		}
		else
		{
			monster->ChangeState(Protocol::kMonsterState::kIdleA);
		}
	}
}
