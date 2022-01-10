#pragma once
#include "data_reader/data_reader_manager.h"

class MonsterStat
{
public:
	explicit MonsterStat(int32_t monsterId);
	~MonsterStat() = default;

public:
	auto GetHp()const->int32_t;
	auto GainHp(int32_t hp)->void;
	auto GetMaxHp()const->int32_t;

	auto SkillToUse(float distance)->std::shared_ptr<DataReaderManager::Skill>;

public:
	static auto Create(int32_t monsterId)->std::shared_ptr<MonsterStat>;

private:
	auto NativeConstruct(int32_t monsterId)->bool;

private:
	int32_t _monster_id;

	int32_t _hp, _max_hp;
	std::vector<std::pair<int32_t, int32_t>> _skills;
	std::vector<std::shared_ptr<DataReaderManager::Skill>>	_skill_info;

};

