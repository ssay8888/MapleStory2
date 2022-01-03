#pragma once
#include "data_reader/data_reader_manager.h"

class MonsterStats : public std::enable_shared_from_this<MonsterStats>
{
public:
	explicit MonsterStats(std::shared_ptr<DataReaderManager::MonsterInfo> monsterInfo);
	~MonsterStats() = default;

private:
	std::shared_ptr<DataReaderManager::MonsterInfo> _monster_info;
	int32_t _hp;

};

