#pragma once
class CharacterStat
{
private:
	CharacterStat();

	auto NativeConstruct()->HRESULT;
public:
	static auto GetInstance() -> CharacterStat&
	{
		static CharacterStat instance;
		return instance;
	}

public:
	auto GetStr()const->int32_t;
	auto GetDex()const->int32_t;
	auto GetInt()const->int32_t;
	auto GetLuk()const->int32_t;

	auto GetHp()const->int32_t;
	auto GetMaxHp()const->int32_t;
	auto GetMp()const->int32_t;
	auto GetMaxMp()const->int32_t;
	auto GetLevel()const->int32_t;
	auto GetExp()const->int32_t;

private:
	int32_t _str, _dex, _int, _luk;
	int32_t _hp, _max_hp, _mp, _max_mp;
	int32_t _level, _exp;
};

