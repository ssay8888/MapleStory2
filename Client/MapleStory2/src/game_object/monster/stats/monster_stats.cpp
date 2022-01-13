#include "c_pch.h"
#include "monster_stats.h"

MonsterStats::MonsterStats(Protocol::GameServerRespawnMonster monsterInfo):
	_hp(monsterInfo.hp()),
	_is_dead(false)
{
}

auto MonsterStats::GetHp() const -> int32_t
{
	return _hp;
}

auto MonsterStats::SetHp(const int32_t value) -> void
{
	_hp = value;
	if (_hp <= 0)
	{
		_is_dead = true;
		_hp = 0;
	}
}

auto MonsterStats::GainHp(const int32_t value) -> void
{
	_hp += value;
	if (_hp <= 0)
	{
		_is_dead = true;
		_hp = 0;
	}
}

auto MonsterStats::IsDead() const -> bool
{
	return _is_dead;
}
