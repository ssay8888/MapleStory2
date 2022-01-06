#include "c_pch.h"
#include "monster_stats.h"

MonsterStats::MonsterStats(Protocol::GameServerRespawnMonster monsterInfo):
	_hp(monsterInfo.hp())
{
}
