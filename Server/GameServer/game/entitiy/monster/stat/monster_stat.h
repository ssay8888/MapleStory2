#pragma once
class MonsterStat
{
public:
	explicit MonsterStat(int32_t monsterId);
	~MonsterStat() = default;

public:
	auto GetHp()const->int32_t;
	auto GainHp(int32_t hp)->void;
	auto GetMaxHp()const->int32_t;

public:
	static auto Create(int32_t monsterId)->std::shared_ptr<MonsterStat>;

private:
	auto NativeConsturct(int32_t monsterId)->bool;

private:
	int32_t _monster_id;

	int32_t _hp, _max_hp;

};

