#pragma once
#include <map>
#include <memory>
#include <string>

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
	auto LoadItemModel(const std::string& path)->void;
	auto FindItemModel(const int32_t itemId)->std::shared_ptr<ItemModel>;
private:
	std::map<int32_t, std::shared_ptr<ItemModel>> _item_model;
};

