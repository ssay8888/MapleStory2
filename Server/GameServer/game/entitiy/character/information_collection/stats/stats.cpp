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
