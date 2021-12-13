#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

class ItemModel;

class DataReaderManager
{
private:
	DataReaderManager() = default;
	~DataReaderManager() = default;

public:
	static auto GetInstance()->DataReaderManager&
	{
		static DataReaderManager instance;
		return instance;
	}

	auto Init()->void;

public:
#pragma region ItemInfoMethod
	auto LoadItemModel(const std::string& path)->void;
	auto FindItemModel(const int32_t itemId)->std::shared_ptr<ItemModel>;
	auto AllItemModel() const ->const std::map<int32_t, std::shared_ptr<ItemModel>>&;
#pragma endregion

#pragma region AnimationLoader

	struct AnimationInfo
	{
		std::string animation_name;
		std::string target_node;
	};
	auto LoadAnimationInfo()->void;
	auto FindAnimationInfo(const std::string& name)->std::shared_ptr<AnimationInfo>;
	auto AllAnimationName()->std::vector<std::shared_ptr<AnimationInfo>>;

#pragma endregion

private:
	std::map<int32_t, std::shared_ptr<ItemModel>> _item_model;
	std::map<std::string, std::shared_ptr<AnimationInfo>> _animations_info;
};

