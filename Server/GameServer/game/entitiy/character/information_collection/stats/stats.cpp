#include "game_server_pch.h"
#include "stats.h"

Stats::Stats(const int64_t characterId):
	BaseInfo(characterId)

{
}

auto Stats::GetStr() const -> int32_t
{
	return _str;
}

auto Stats::SetStr(const int32_t str) -> void
{
	_str = str;
}

auto Stats::GetDex() const -> int32_t
{
	return _dex;
}

auto Stats::SetDex(const int32_t dex) -> void
{
	_dex = dex;
}

auto Stats::GetInt() const -> int32_t
{
	return _int;
}

auto Stats::SetInt(const int32_t int_) -> void
{
	_int = int_;
}


auto Stats::GetLuk() const -> int32_t
{
	return _luk;
}

auto Stats::SetLuk(const int32_t luk) -> void
{
	_luk = luk;
}

auto Stats::GetHp() const -> int32_t
{
	return _hp;
}

auto Stats::SetHp(const int32_t hp) -> void
{
	_hp = hp;
}

auto Stats::GainHp(int32_t hp) -> void
{
	_hp += hp;
}

auto Stats::GetMaxHp() const -> int32_t
{
	return _max_hp;
}

auto Stats::SetMaxHp(const int32_t maxHp) -> void
{
	_max_hp = maxHp;
}

auto Stats::GetMp() const -> int32_t
{
	return _mp;
}

auto Stats::SetMp(const int32_t mp) -> void
{
	_mp = mp;
}

auto Stats::GetMaxMp() const -> int32_t
{
	return _max_mp;
}

auto Stats::SetMaxMp(const int32_t maxMp) -> void
{
	_max_mp = maxMp;
}

auto Stats::GetLevel() const -> int32_t
{
	return _level;
}

auto Stats::SetLevel(const int32_t level) -> void
{
	_level = level;
}

auto Stats::GetExp() const -> int32_t
{
	return _exp;
}

auto Stats::SetExp(const int32_t exp) -> void
{
	_exp = exp;
}

auto Stats::Create(const int64_t characterId) -> std::shared_ptr<Stats>
{
	auto instance = MakeShared<Stats>(characterId);

	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}

	return instance;
}

auto Stats::NativeConstruct() -> HRESULT
{
	return S_OK;
}
