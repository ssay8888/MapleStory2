#include "game_server_pch.h"
#include "monster_attack_a_state.h"

#include "game/entitiy/character/game_character.h"
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

	ReloadMapObject(monster, _map_objects.empty());
	monster->GetMonsterColliderAabb()->UpdateCollider();
	auto range = monster->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
}

auto MonsterAttackAState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc += timeDelta;
	if (wait_skill_delay == 0)
	{
		if (monster->GetUseSkill() != nullptr)
		{
			int index = 0;
			for (const auto& motion : monster->GetUseSkill()->motions)
			{
				if (motion->movedistance > 0)
				{
					const auto transform = monster->GetTransform();
					
					transform->LookAtTarget(monster->GetTargetCharacter()->GetTransform()->GetState(Transform::kState::kStatePosition));

					for (int i = 0; i < 10; ++i)
					{
						transform->BackStraight(0.1, motion->movedistance);

						ReloadMapObject(monster, _map_objects.empty());
						monster->GetMonsterColliderAabb()->UpdateCollider();
						auto range = monster->GetReloadRangeAabb();
						for (const auto& reload : range)
						{
							reload->UpdateCollider();
						}

						_last_tile_map_object = LoadLastTile(monster);
						if (StraightCheck(monster) || !BlockUpCheck(monster))
						{
							transform->BackStraight(-0.1, motion->movedistance);
						}

					}
				}
				index++;
			}

			Protocol::GameServerMoveMonster sendPkt;
			std::cout << "attack " << _state <<  "/ " << monster->GetObjectId() << std::endl;
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
	}
}

auto MonsterAttackAState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	if (wait_skill_delay == 0)
	{
		auto aniKey = DataReaderManager::GetInstance().FindAniKey(monster->GetSpawnPoint()->GetSpawnNpcId());
		for (auto seq : aniKey->seqs)
		{

			if ((seq.second->name == L"Attack_01_A" && _state == Protocol::kAttack1A) ||
				(seq.second->name == L"Attack_02_A" && _state == Protocol::kAttack1A) ||
				(seq.second->name == L"Attack_03_A" && _state == Protocol::kAttack1A) ||
				(seq.second->name == L"Attack_01_B" && _state == Protocol::kAttack1B) ||
				(seq.second->name == L"Attack_02_B" && _state == Protocol::kAttack1B) ||
				(seq.second->name == L"Attack_03_B" && _state == Protocol::kAttack1B) ||
				(seq.second->name == L"Attack_01_C" && _state == Protocol::kAttack1C) ||
				(seq.second->name == L"Attack_02_C" && _state == Protocol::kAttack1C) ||
				(seq.second->name == L"Attack_03_C" && _state == Protocol::kAttack1C))
			{
				wait_skill_delay += seq.second->key[L"end"];
			}
		}
	}

	if (_animation_acc >= wait_skill_delay)
	{
		monster->ChangeState(Protocol::kMonsterState::kIdleA);
	}

}
