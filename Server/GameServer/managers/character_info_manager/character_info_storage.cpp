#include "game_server_pch.h"
#include "character_info_storage.h"

CharacterInfoStorage::CharacterInfoStorage(const kInfoTypes type) :
	_type(type)
{
}

auto CharacterInfoStorage::PushInfo(const int64_t characterId, const std::shared_ptr<BaseInfo>& info) -> bool
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<BaseInfo>>::accessor result;
	if (_information.insert(result, characterId))
	{
		result->second = info;
		return true;
	}
	return false;
}

auto CharacterInfoStorage::FindInfo(const int64_t characterId) const -> std::shared_ptr<BaseInfo>
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<BaseInfo>>::const_accessor result;
	if (_information.find(result, characterId))
	{
		return result->second;
	}
	return nullptr;
}

auto CharacterInfoStorage::RemoveInfo(const int64_t characterId) -> bool
{
	return _information.erase(characterId);
}
