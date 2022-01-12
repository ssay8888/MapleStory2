#include "c_pch.h"
#include "monster_attack_a_state.h"

#include "data_reader/data_reader_manager.h"
#include "randomizer/randomizer.h"
#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/monster/monster.h"
#include "src/game_object/player/player.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"

MonsterAttackAState::MonsterAttackAState(Protocol::kMonsterState state):
	_state(state)
{
}

auto MonsterAttackAState::Enter(std::shared_ptr<Monster> monster) -> void
{
	_min_distance = -1;
	_is_player_attack = false;
	if (_aabb_com)
	{
		_aabb_com->ResetCollider();
	}
	if (_skills.empty())
	{
		/*
		auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monster->GetMonsterInfo().monster_id());
		_skills = monsterInfo->skills;
		for (const auto skill : _skills)
		{
			const auto skilldata = DataReaderManager::GetInstance().FindSkillData(skill.first);
			_use_motions = skilldata->motions;
		}*/
		const auto skilldata = DataReaderManager::GetInstance().FindSkillData(monster->GetSkillId());
		_use_motions = skilldata->motions;

		for (auto skill : _use_motions)
		{
			if ((skill->sequence_name == L"attack_01_a" && _state == Protocol::kAttack1A) ||
				(skill->sequence_name == L"attack_02_a" && _state == Protocol::kAttack2A) ||
				(skill->sequence_name == L"attack_03_a" && _state == Protocol::kAttack3A) ||
				(skill->sequence_name == L"attack_01_b" && _state == Protocol::kAttack1B) ||
				(skill->sequence_name == L"attack_02_b" && _state == Protocol::kAttack2B) ||
				(skill->sequence_name == L"attack_03_b" && _state == Protocol::kAttack3B) ||
				(skill->sequence_name == L"attack_01_c" && _state == Protocol::kAttack1C) ||
				(skill->sequence_name == L"attack_02_c" && _state == Protocol::kAttack2C) ||
				(skill->sequence_name == L"attack_03_c" && _state == Protocol::kAttack3C))
			{
				_use_skill = skill;
			}
		}
		if (_use_skill != nullptr && _aabb_com == nullptr)
		{
			auto transform = monster->GetTransform();
			Collider::TagColliderDesc		ColliderDesc;
			ColliderDesc.parent_matrix = &transform->GetWorldMatrix();
			ColliderDesc.scale = _float3(
				(_use_skill->attack.range.width == 0 ? _use_skill->attack.range.distance : _use_skill->attack.range.width),
				_use_skill->attack.range.distance,
				_use_skill->attack.range.height);
			ColliderDesc.init_pos = _float3(0.f, (_use_skill->attack.range.distance) * 0.5f, 0.f);

			const auto& componentManager = ComponentManager::GetInstance();
			auto component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
			_aabb_com = std::static_pointer_cast<Collider>(component);
			_aabb_com->NativeConstruct(&ColliderDesc);
			_aabb_com->UpdateCollider();
		}
	}
}

auto MonsterAttackAState::HandleInput() -> void
{
}

auto MonsterAttackAState::Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	if (_use_skill)
	{

		auto& dataReaderManager = DataReaderManager::GetInstance();

		const auto transform = monster->GetTransform();
		const auto kfm = dataReaderManager.FindAniKey(monster->GetMonsterInfo().monster_id());
		const auto index = monster->GetStateIndex(_state);
		const auto seq = kfm->seqs[index];


		if (kfm)
		{
			auto p0 = seq->key[L"p0"];
			const auto player = std::static_pointer_cast<Player>(ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, TEXT("Layer_Character"), 0));
			if (player)
			{
				if (monster->GetAnimationTimeAcc() >= p0 && !_is_player_attack)
				{
					_is_player_attack = _aabb_com->CollisionAabb(player->GetCharacterColliderAabb());
					Protocol::GameClientTakeDamage sendPkt;
					
					sendPkt.set_character_id(GameLogicQueue::GetInstance()->GetCharacterInfo().character_id());
					sendPkt.set_monster_obj_id(monster->GetMonsterInfo().object_id());
					SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
				}
			}
		}
		if (_use_skill->movedistance > 0)
		{
			const auto endTime = seq->key[L"end"];

			transform->BackStraight(timeDelta + timeDelta * endTime, _use_skill->movedistance);

			monster->GetMonsterColliderAabb()->UpdateCollider();
			auto range = monster->GetReloadRangeAabb();
			for (const auto& reload : range)
			{
				reload->UpdateCollider();
			}
			_last_tile_map_object = LoadLastTile(monster);
			if (StraightCheck(monster) || !BlockUpCheck(monster))
			{
				transform->BackStraight(timeDelta + timeDelta * endTime, _use_skill->movedistance);
			}

			_float3 position = transform->GetState(Transform::kState::kStatePosition);
			_float3	dir = _targetPos - position;
			const float	distance = D3DXVec3Length(&dir);
			D3DXVec3Normalize(&dir, &dir);


			if (_min_distance == -1) // 첫 거리구해옴
			{
				_min_distance = distance;
			}

			if (_min_distance >= distance) // 거리가좁아졌다면 갱신
			{
				_min_distance = distance;
			}
			else // 거리가 더멀어져..?
			{
				transform->SetState(Transform::kState::kStatePosition, _targetPos);
			}

			if (distance >= _use_skill->movedistance)
			{
				transform->SetState(Transform::kState::kStatePosition, _targetPos);
			}
		}
	}
}

auto MonsterAttackAState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{


	monster->PlayAnimation(timeDelta);
	if (_use_skill != nullptr)
	{
		_aabb_com->UpdateCollider();
	}
	auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetMonsterInfo().monster_id());

	auto index = monster->GetStateIndex(_state);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (monster->GetAnimationTimeAcc() >= endTime)
	{
		std::cout << monster->GetAnimationTimeAcc() << "/" << endTime << std::endl;
		if (_state == Protocol::kAttack1A)
		{
			if (_use_motions.size() >= 2)
			{
				monster->ChangeAnimation(Protocol::kAttack2A);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1A);
			}
		}
		else if (_state == Protocol::kAttack2A)
		{
			if (_use_motions.size() >= 3)
			{
				monster->ChangeAnimation(Protocol::kAttack3A);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1A);
			}
		}
		else if (_state == Protocol::kAttack3A)
		{
			monster->ChangeAnimation(Protocol::kAttack1A);
		}
		else if (_state == Protocol::kAttack1B)
		{
			if (_use_motions.size() >= 2)
			{
				monster->ChangeAnimation(Protocol::kAttack2B);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1B);
			}
		}
		else if (_state == Protocol::kAttack2B)
		{
			if (_use_motions.size() >= 3)
			{
				monster->ChangeAnimation(Protocol::kAttack3B);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1B);
			}
		}
		else if (_state == Protocol::kAttack3B)
		{
			monster->ChangeAnimation(Protocol::kAttack1B);
		}
		else if (_state == Protocol::kAttack1C)
		{
			if (_use_motions.size() >= 2)
			{
				monster->ChangeAnimation(Protocol::kAttack2C);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1C);
			}
		}
		else if (_state == Protocol::kAttack2C)
		{
			if (_use_motions.size() >= 3)
			{
				monster->ChangeAnimation(Protocol::kAttack3C);
			}
			else
			{
				monster->ChangeAnimation(Protocol::kAttack1C);
			}
		}
		else if (_state == Protocol::kAttack3C)
		{
			monster->ChangeAnimation(Protocol::kAttack1C);
		}
	}
}

auto MonsterAttackAState::Render(std::shared_ptr<Monster> monster) -> void
{
	auto& dataReaderManager = DataReaderManager::GetInstance();

	_aabb_com->RenderDebug();

	//TODO : 이펙트가 출력되는 시간.
	/*if (const auto& motionEffect = dataReaderManager.FindMotionEffect(_use_skill))
	{
		if (motionEffect->type == DataReaderManager::kEffectType::kNif)
		{
			if (monster->GetAnimationTimeAcc() <= motionEffect->duration)
			{
			}
		}
	}
	const auto& tarEffects = dataReaderManager.FindStrTagEffect(_use_skill);
	if (!tarEffects.empty())
	{
		for (const auto& motion : tarEffects)
		{
			if (motion->type == DataReaderManager::kEffectType::kNif)
			{
				if (monster->GetAnimationTimeAcc() <= motion->duration)
				{
					_aabb_com->RenderDebug();
				}
			}
		}
	}*/
}

auto MonsterAttackAState::GetTargetPos() const -> _float3
{
	return _targetPos;
}

auto MonsterAttackAState::SetTargetPos(const _float3 pos) -> void
{
	_targetPos = pos;
}
