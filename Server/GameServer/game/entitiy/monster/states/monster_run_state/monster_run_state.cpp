#include "game_server_pch.h"
#include "monster_run_state.h"

#include "data_reader/data_reader_manager.h"
#include "game/entitiy/character/game_character.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/entitiy/monster/stat/monster_stat.h"
#include "game/map/map_instance.h"
#include "protocol/game_enum.pb.h"
#include "protocol/game_protocol.pb.h"
#include "randomizer/randomizer.h"
#include "src/utility/components/transform/transform.h"

auto MonsterRunState::Enter(std::shared_ptr<GameMonster> monster) -> void
{
	auto range = monster->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
	monster->GetMonsterColliderAabb()->UpdateCollider();
	ReloadMapObject(monster, true);
}

auto MonsterRunState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	if (monster->GetTargetCharacter() == nullptr || monster->GetTargetCharacter()->GetTransform() == nullptr)
	{
		return;
	}

	_animation_acc += timeDelta;
	const auto transform = monster->GetTransform();
	const auto skill = monster->GetStat()->SkillToUse(GetTargetDistance(monster));

	auto distance = GetTargetDistance(monster);
	
	if (0.3 <= distance || skill == nullptr /*|| !CheckTargetCharacterDistance(monster, 0.2f)*/) //공격범위안에있는지체크해야할곳.
	{
		auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monster->GetSpawnPoint()->GetSpawnNpcId());
		transform->LookAtTarget(monster->GetTargetCharacter()->GetTransform()->GetState(Transform::kState::kStatePosition));

		transform->BackStraight(timeDelta, monsterInfo->model.run_speed);

		monster->GetMonsterColliderAabb()->UpdateCollider();
		auto range = monster->GetReloadRangeAabb();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}
		ReloadMapObject(monster, _map_objects.empty());
		_last_tile_map_object = LoadLastTile(monster);
		if (StraightCheck(monster) || !BlockUpCheck(monster))
		{
			transform->BackStraight(-timeDelta, monsterInfo->model.run_speed);
		}
		else
		{
			Protocol::GameServerMoveMonster sendPkt;
			sendPkt.set_object_id(monster->GetObjectId());
			std::cout << "kRunA " << monster->GetObjectId() << std::endl;
			sendPkt.set_state(Protocol::kRunA);

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
		monster->GetMonsterColliderAabb()->UpdateCollider();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}
	}
}

auto MonsterRunState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monster->GetSpawnPoint()->GetSpawnNpcId());

	auto distance = GetTargetDistance(monster);

	if (monsterInfo != nullptr && 0.3 >= distance)
	{
		const auto skill = monster->GetStat()->SkillToUse(distance);
		if (skill != nullptr && !skill->motions.empty())
		{
			if (skill->motions[0]->sequence_name.find(L"_a") != std::wstring::npos)
			{
				monster->ChangeUseSkill(skill);
				monster->ChangeState(Protocol::kMonsterState::kAttack1A);
			}
			else if (skill->motions[0]->sequence_name.find(L"_b") != std::wstring::npos)
			{
				monster->ChangeUseSkill(skill);
				monster->ChangeState(Protocol::kMonsterState::kAttack1B);
			}
			else if (skill->motions[0]->sequence_name.find(L"_c") != std::wstring::npos)
			{
				monster->ChangeUseSkill(skill);
				monster->ChangeState(Protocol::kMonsterState::kAttack1C);
			}
			else
			{
				std::cout << "알 수 없는 모션" << std::endl;
			}
			return;
		}
	}

	if (!CheckTargetCharacterDistance(monster, 2.f))
	{
		monster->ChangeState(Protocol::kMonsterState::kIdleA);
	}
}
