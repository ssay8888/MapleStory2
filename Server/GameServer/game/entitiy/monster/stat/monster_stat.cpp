#include "game_server_pch.h"
#include "monster_stat.h"

#include "data_reader/data_reader_manager.h"

MonsterStat::MonsterStat(const int32_t monsterId):
	_monster_id(monsterId),
	_hp(0),
	_max_hp(0)
{
}

auto MonsterStat::GetHp() const -> int32_t
{
	return _hp;
}

auto MonsterStat::GainHp(int32_t hp) -> void
{
	_hp += hp;
}

auto MonsterStat::GetMaxHp() const -> int32_t
{
	return _max_hp;
}

auto MonsterStat::Create(int32_t monsterId) -> std::shared_ptr<MonsterStat>
{
	auto instance = std::make_shared<MonsterStat>(monsterId);
	if (!instance->NativeConsturct(monsterId))
	{
		return nullptr;
	}
	return instance;
}

auto MonsterStat::NativeConsturct(const int32_t monsterId) -> bool
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monsterId);

	_max_hp = _hp = monsterInfo->stat.hp;

	return true;
}
