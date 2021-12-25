#pragma once
#include "game/entitiy/character/information_collection/base_info.h"

class Stats : public BaseInfo
{
public:
	explicit Stats(int64_t characterId);

public:
	auto GetStr()const->int32_t;
	auto SetStr(int32_t str)->void;

	auto GetDex()const->int32_t;
	auto SetDex(int32_t dex)->void;

	auto GetInt()const->int32_t;
	auto SetInt(int32_t int_)->void;

	auto GetLuk()const->int32_t;
	auto SetLuk(int32_t luk)->void;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<Stats>;

private:
	auto NativeConstruct()->HRESULT;

private:
	int32_t _str, _dex, _int, _luk;
};

