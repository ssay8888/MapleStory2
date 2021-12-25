#pragma once
class BaseInfo;

class CharacterInfoStorage
{
public:
	enum class kInfoTypes : int16_t
	{
		kStats,
		kInventory,
		kEnd
	};

	explicit CharacterInfoStorage(kInfoTypes type);
	~CharacterInfoStorage() = default;

public:
	auto PushInfo(int64_t characterId, const std::shared_ptr<BaseInfo>& info)->bool;
	auto FindInfo(int64_t characterId) const ->std::shared_ptr<BaseInfo>;
	auto RemoveInfo(int64_t characterId)->bool;

private:
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<BaseInfo>>	_information;


	kInfoTypes	_type;
};

