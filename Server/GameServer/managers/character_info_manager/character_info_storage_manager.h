#pragma once
#include "character_info_storage.h"

class CharacterInfoStorageManager
{
private:
	CharacterInfoStorageManager();

public:
	static auto GetInstance()->CharacterInfoStorageManager&
	{
		static CharacterInfoStorageManager instance;
		return instance;
	}

	auto FindInfo(CharacterInfoStorage::kInfoTypes type, int64_t characterId) const ->std::shared_ptr<BaseInfo>;
	auto PushInfo(CharacterInfoStorage::kInfoTypes type, int64_t characterId, const std::shared_ptr<BaseInfo>& info) const ->bool;

private:
	tbb::concurrent_hash_map<CharacterInfoStorage::kInfoTypes, std::shared_ptr<CharacterInfoStorage>> _information;
};

