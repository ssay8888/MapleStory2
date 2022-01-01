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

	auto GetHp()const->int32_t;
	auto SetHp(int32_t hp)->void;

	auto GetMaxHp()const->int32_t;
	auto SetMaxHp(int32_t maxHp)->void;

	auto GetMp()const->int32_t;
	auto SetMp(int32_t mp)->void;

	auto GetMaxMp()const->int32_t;
	auto SetMaxMp(int32_t maxMp)->void;

	auto GetLevel()const->int32_t;
	auto SetLevel(int32_t level)->void;

	auto GetExp()const->int32_t;
	auto SetExp(int32_t exp)->void;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<Stats>;

private:
	auto NativeConstruct()->HRESULT;

private:
	int32_t _str, _dex, _int, _luk;
	int32_t _hp, _max_hp, _mp, _max_mp;
	int32_t _level, _exp;
};

