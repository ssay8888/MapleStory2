#include "c_pch.h"
#include "character_stat.h"

#include "src/game_job_queue/game_logic_queue.h"

CharacterStat::CharacterStat()
{
	NativeConstruct();
}

auto CharacterStat::NativeConstruct() -> HRESULT
{
	const auto gameLogicManager = GameLogicQueue::GetInstance();
	const auto characterInfo = gameLogicManager->GetCharacterInfo();

	_str = characterInfo.str();
	_dex = characterInfo.dex();
	_int = characterInfo.int_();
	_luk = characterInfo.luk();

	_hp = characterInfo.hp();
	_max_hp = characterInfo.max_hp();
	_mp = characterInfo.mp();
	_max_mp = characterInfo.max_mp();

	_level = characterInfo.level();

	_exp = characterInfo.exp();
	return S_OK;
}

auto CharacterStat::GetStr() const -> int32_t
{
	return _str;
}

auto CharacterStat::GetDex() const -> int32_t
{
	return _dex;
}

auto CharacterStat::GetInt() const -> int32_t
{
	return _int;
}

auto CharacterStat::GetLuk() const -> int32_t
{
	return _luk;
}

auto CharacterStat::GetHp() const -> int32_t
{
	return _hp;
}

auto CharacterStat::SetHp(const int32_t value) -> void
{
	_hp = value;
}

auto CharacterStat::GainHp(const int32_t value) -> void
{
	_hp += value;
}

auto CharacterStat::GetMaxHp() const -> int32_t
{
	return _max_hp;
}

auto CharacterStat::GetMp() const -> int32_t
{
	return _mp;
}

auto CharacterStat::SetMp(const int32_t value) -> void
{
	_mp = value;
}

auto CharacterStat::GainMp(const int32_t value) -> void
{
	_mp += value;
}

auto CharacterStat::GetMaxMp() const -> int32_t
{
	return _max_mp;
}

auto CharacterStat::GetLevel() const -> int32_t
{
	return _level;
}

auto CharacterStat::GetExp() const -> int32_t
{
	return _exp;
}

auto CharacterStat::SetExp(const int32_t value) -> void
{
	_exp = value;
}

auto CharacterStat::GainExp(const int32_t value) -> void
{
	_exp += value;
}
