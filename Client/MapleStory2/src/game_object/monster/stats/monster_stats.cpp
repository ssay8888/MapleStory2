#include "c_pch.h"
#include "monster_stats.h"

MonsterStats::MonsterStats(std::shared_ptr<DataReaderManager::MonsterInfo> monsterInfo):
	_monster_info(monsterInfo),
	_hp(monsterInfo->stat.hp)
{
}
