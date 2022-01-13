#include "game_server_pch.h"
#include "monster_stat.h"

#include "randomizer/randomizer.h"

MonsterStat::MonsterStat(const int32_t monsterId):
	_monster_id(monsterId),
	_hp(0),
	_max_hp(0),
	_is_dead(false)
{
	const auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(_monster_id);
	_skills = monsterInfo->skills;

	for (auto [skillId, prob] : monsterInfo->skills)
	{
		if (auto skill = DataReaderManager::GetInstance().FindSkillData(skillId))
		{
			_skill_info.push_back(skill);
		}
	}
}

auto MonsterStat::GetHp() const -> int32_t
{
	return _hp;
}

auto MonsterStat::GainHp(const int32_t hp) -> void
{
	if (_hp + hp <= 0)
	{
		_is_dead = true;
		_hp = 0;
		return;
	}
	_hp += hp;
}

auto MonsterStat::GetMaxHp() const -> int32_t
{
	return _max_hp;
}

auto MonsterStat::SkillToUse(const float distance) -> std::shared_ptr<DataReaderManager::Skill>
{
	const auto rand = Randomizer::GetInstance().Rand(1ll, 100ll);

	auto accProb = 0;
	size_t index = 0;
	for (const auto& [skillId, prob] : _skills)
	{
		if (index >= _skill_info.size())
		{
			return nullptr;
		}
		accProb += prob;
		if (accProb >= rand)
		{
			if (_skill_info[index]->detect.distance >= distance)
			{
				return _skill_info[index];
			}
		}
		++index;
	}

	return nullptr;
}

auto MonsterStat::IsDead() const -> bool
{
	return _is_dead;
}

auto MonsterStat::Create(int32_t monsterId) -> std::shared_ptr<MonsterStat>
{
	auto instance = std::make_shared<MonsterStat>(monsterId);
	if (!instance->NativeConstruct(monsterId))
	{
		return nullptr;
	}
	return instance;
}

auto MonsterStat::NativeConstruct(const int32_t monsterId) -> bool
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monsterId);

	_max_hp = _hp = monsterInfo->stat.hp;

	return true;
}
