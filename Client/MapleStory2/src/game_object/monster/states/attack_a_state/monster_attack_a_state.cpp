#include "c_pch.h"
#include "monster_attack_a_state.h"

#include "data_reader/data_reader_manager.h"
#include "randomizer/randomizer.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

MonsterAttackAState::MonsterAttackAState(Protocol::kMonsterState state):
	_state(state)
{
}

auto MonsterAttackAState::Enter(std::shared_ptr<Monster> monster) -> void
{
	ReloadMapObject(monster);
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
				(skill->sequence_name == L"attack_03_a" && _state == Protocol::kAttack3A))
			{
				_use_skill = skill;
			}
			else if ((skill->sequence_name == L"attack_01_b" && _state == Protocol::kAttack1B) ||
				(skill->sequence_name == L"attack_02_b" && _state == Protocol::kAttack2B) ||
				(skill->sequence_name == L"attack_03_b" && _state == Protocol::kAttack3B))
			{
				_use_skill = skill;
			}
			else if ((skill->sequence_name == L"attack_01_c" && _state == Protocol::kAttack1C) ||
				(skill->sequence_name == L"attack_02_c" && _state == Protocol::kAttack2C) ||
				(skill->sequence_name == L"attack_03_c" && _state == Protocol::kAttack3C))
			{
				_use_skill = skill;
			}
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
		if (_use_skill->movedistance > 0)
		{

			auto transform = monster->GetTransform();
			{
				auto kfm = DataReaderManager::GetInstance().FindAniKey(monster->GetMonsterInfo().monster_id());

				auto index = monster->GetStateIndex(_state);
				auto seq = kfm->seqs[index];
				auto endTime = seq->key[L"end"];
				transform->BackStraight(timeDelta * endTime, _use_skill->movedistance);

				monster->GetMonsterColliderAabb()->UpdateCollider();
				auto range = monster->GetReloadRangeAabb();
				for (const auto& reload : range)
				{
					reload->UpdateCollider();
				}
				_last_tile_map_object = LoadLastTile(monster);
				if (StraightCheck(monster) || !BlockUpCheck(monster))
				{
					transform->BackStraight(timeDelta * endTime, _use_skill->movedistance);
				}
			}
		}
	}
}

auto MonsterAttackAState::LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void
{
	monster->PlayAnimation(timeDelta);
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
}
