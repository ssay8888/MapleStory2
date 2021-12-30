#include "game_server_pch.h"
#include "character_info_storage_manager.h"

CharacterInfoStorageManager::CharacterInfoStorageManager()
{
	constexpr auto maxCount = static_cast<int32_t>(CharacterInfoStorage::kInfoTypes::kEnd);
	for (int32_t i = 0; i < maxCount; ++i)
	{
		auto type = static_cast<CharacterInfoStorage::kInfoTypes>(i);
		_information.emplace(type, std::make_shared<CharacterInfoStorage>(type));
	}
}

auto CharacterInfoStorageManager::FindInfo(const CharacterInfoStorage::kInfoTypes type,
                                           const int64_t characterId) const -> std::shared_ptr<BaseInfo>
{
	tbb::concurrent_hash_map<CharacterInfoStorage::kInfoTypes, std::shared_ptr<CharacterInfoStorage>>::const_accessor result;
	if (_information.find(result, type))
	{
		if (auto item = result->second->FindInfo(characterId))
		{
			return item;
		}
	}
	return nullptr;
}

auto CharacterInfoStorageManager::PushInfo(const CharacterInfoStorage::kInfoTypes type, const int64_t characterId,
                                           const std::shared_ptr<BaseInfo>& info) const -> bool
{
	tbb::concurrent_hash_map<CharacterInfoStorage::kInfoTypes, std::shared_ptr<CharacterInfoStorage>>::const_accessor result;
	if (_information.find(result, type))
	{
		if (const auto check = result->second->PushInfo(characterId, info))
		{
			return check;
		}
	}
	return false;
}

auto CharacterInfoStorageManager::RemoveInfo(const CharacterInfoStorage::kInfoTypes type, const int64_t characterId) const ->bool
{
	tbb::concurrent_hash_map<CharacterInfoStorage::kInfoTypes, std::shared_ptr<CharacterInfoStorage>>::const_accessor result;
	if (_information.find(result, type))
	{
		if (const auto check = result->second->RemoveInfo(characterId))
		{
			return check;
		}
	}
	return false;
}

auto CharacterInfoStorageManager::RemoveAllInfo(const int64_t characterId) const ->bool
{
	for (auto i = 0; i < static_cast<int32_t>(CharacterInfoStorage::kInfoTypes::kEnd); ++i)
	{
		tbb::concurrent_hash_map<CharacterInfoStorage::kInfoTypes, std::shared_ptr<CharacterInfoStorage>>::const_accessor result;
		if (_information.find(result, static_cast<CharacterInfoStorage::kInfoTypes>(i)))
		{
			result->second->RemoveInfo(characterId);
		}
	}
	return true;
}
