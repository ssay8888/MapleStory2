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

	_ap = characterInfo.ap();

	SetMapId(characterInfo.map());
	//SetMapId(2000230);
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

auto CharacterStat::SetStr(int32_t value) -> void
{
	_str = value;
}

auto CharacterStat::SetDex(int32_t value) -> void
{
	_dex = value;
}

auto CharacterStat::SetInt(int32_t value) -> void
{
	_int = value;
}

auto CharacterStat::SetLuk(int32_t value) -> void
{
	_luk = value;
}

auto CharacterStat::GetHp() const -> int32_t
{
	return _hp;
}

auto CharacterStat::SetHp(const int32_t value) -> void
{
	_hp = value;
	_is_dead = false;
	if (_hp <= 0)
	{
		_is_dead = true;
	}
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

auto CharacterStat::SetLevel(const int32_t level) -> void
{
	_level = level;
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

auto CharacterStat::GetAp() const -> int32_t
{
	return _ap;
}

auto CharacterStat::SetAp(int32_t value) -> void
{
	_ap = value;
}

auto CharacterStat::GetMapId() const -> int32_t
{
	return _map->id;
}

auto CharacterStat::SetMapId(int32_t id) -> void
{
	auto data = DataReaderManager::GetInstance().FindMapName(id);
	if (data)
	{
		_map = data;
	}
	else
	{
		_map = nullptr;
	}
}

auto CharacterStat::GetMapName() const -> std::wstring
{
	return _map->environment.name;
}

auto CharacterStat::IsDead() const -> bool
{
	return _is_dead;
}
