#pragma once
#include "data_reader/data_reader_manager.h"
#include "protocol/game_protocol.pb.h"

class MonsterStats : public std::enable_shared_from_this<MonsterStats>
{
public:
	explicit MonsterStats(Protocol::GameServerRespawnMonster monsterInfo);
	~MonsterStats() = default;

private:

	std::shared_ptr<DataReaderManager::MonsterInfo> _monster_info;
	int32_t _hp;

};

