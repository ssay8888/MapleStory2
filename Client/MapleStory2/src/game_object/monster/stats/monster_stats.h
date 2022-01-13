#pragma once
#include "data_reader/data_reader_manager.h"
#include "protocol/game_protocol.pb.h"

class MonsterStats : public std::enable_shared_from_this<MonsterStats>
{
public:
	explicit MonsterStats(Protocol::GameServerRespawnMonster monsterInfo);
	~MonsterStats() = default;

public:
	auto GetHp()const->int32_t;
	auto SetHp(int32_t value)->void;
	auto GainHp(int32_t value)->void;

	auto IsDead()const->bool;

private:
	std::shared_ptr<DataReaderManager::MonsterInfo> _monster_info;
	int32_t _hp;

	bool _is_dead;
};

