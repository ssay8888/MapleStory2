#include "game_server_pch.h"
#include "monster_attack_a_state.h"

#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/map/map_instance.h"
#include "protocol/game_protocol.pb.h"
#include "randomizer/randomizer.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

MonsterAttackAState::MonsterAttackAState(Protocol::kMonsterState state) :
	_state(state)
{
	
}
auto MonsterAttackAState::Enter(std::shared_ptr<GameMonster> monster) -> void
{
	wait_skill_delay = 0.f;
	_animation_acc = 0.f;

	Protocol::GameServerMoveMonster sendPkt;
	sendPkt.set_object_id(monster->GetObjectId());
	sendPkt.set_state(_state);

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

	sendPkt.set_skillid(monster->GetUseSkill()->id);

	monster->GetMapInstance()->BroadCastMessage(sendPkt, nullptr);
}

auto MonsterAttackAState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc += timeDelta;
	if (wait_skill_delay == 0)
	{
		if (monster->GetUseSkill() != nullptr)
		{
			for (auto motion : monster->GetUseSkill()->motions)
			{
				//motion->movedistance
				//TODO : 이동거리 미구현
			}
		}
	}
}

auto MonsterAttackAState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	if (wait_skill_delay == 0)
	{
		auto aniKey = DataReaderManager::GetInstance().FindAniKey(monster->GetSpawnPoint()->GetSpawnNpcId());
		for (auto seq : aniKey->seqs)
		{
			if (seq.second->name.find(L"Attack") != std::wstring::npos)
			{

				if (seq.second->name.find(L"_A") != std::wstring::npos)
				{
					wait_skill_delay += seq.second->key[L"end"];
				}
			}
		}
	}

	if (_animation_acc >= wait_skill_delay)
	{                                                                           
	}

}
