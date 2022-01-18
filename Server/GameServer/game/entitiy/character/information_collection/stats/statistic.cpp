#include "game_server_pch.h"
#include "statistic.h"

Statistic::Statistic(const int64_t characterId):
	BaseInfo(characterId)

{
}

auto Statistic::GetStr() const -> int32_t
{
	return _str;
}

auto Statistic::SetStr(const int32_t str) -> void
{
	_str = str;
}

auto Statistic::GetDex() const -> int32_t
{
	return _dex;
}

auto Statistic::SetDex(const int32_t dex) -> void
{
	_dex = dex;
}

auto Statistic::GetInt() const -> int32_t
{
	return _int;
}

auto Statistic::SetInt(const int32_t int_) -> void
{
	_int = int_;
}


auto Statistic::GetLuk() const -> int32_t
{
	return _luk;
}

auto Statistic::SetLuk(const int32_t luk) -> void
{
	_luk = luk;
}

auto Statistic::GetHp() const -> int32_t
{
	return _hp;
}

auto Statistic::SetHp(const int32_t hp) -> void
{
	_hp = hp;
	if (_hp > _max_hp)
	{
		_hp = _max_hp;
	}
}

auto Statistic::GainHp(int32_t hp) -> void
{
	_hp += hp;
}

auto Statistic::GetMaxHp() const -> int32_t
{
	return _max_hp;
}

auto Statistic::SetMaxHp(const int32_t maxHp) -> void
{
	_max_hp = maxHp;
}

auto Statistic::GetMp() const -> int32_t
{
	return _mp;
}

auto Statistic::SetMp(const int32_t mp) -> void
{
	_mp = mp;
	if (_mp > _max_mp)
	{
		_mp = _max_mp;
	}
}

auto Statistic::GetMaxMp() const -> int32_t
{
	return _max_mp;
}

auto Statistic::SetMaxMp(const int32_t maxMp) -> void
{
	_max_mp = maxMp;
}

auto Statistic::GetLevel() const -> int32_t
{
	return _level;
}

auto Statistic::SetLevel(const int32_t level) -> void
{
	_level = level;
}

auto Statistic::GetExp() const -> int32_t
{
	return _exp;
}

auto Statistic::GainExp(const int32_t exp) -> void
{
	_exp += exp;
}

auto Statistic::SetExp(const int32_t exp) -> void
{
	_exp = exp;
}

auto Statistic::GetAp() const -> int32_t
{
	return _ap;
}

auto Statistic::SetAp(int32_t value) -> void
{
	_ap = value;
}

auto Statistic::Create(const int64_t characterId) -> std::shared_ptr<Statistic>
{
	auto instance = MakeShared<Statistic>(characterId);

	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}

	return instance;
}

auto Statistic::NativeConstruct() -> HRESULT
{
	return S_OK;
}
