#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

class ItemModel
{
public:
	struct Dummy
	{
		int32_t gender;
		std::string target_node;
		std::string rotation;
		std::string translation;
	};

	struct Asset
	{
		std::string name;
		std::string self_node;
		std::string target_node;
		int32_t		weapon;
		int32_t		gender;
		std::vector<std::shared_ptr<Dummy>> dummies;
	};

	struct Slot
	{
		std::string			name;
		std::vector<std::shared_ptr<Asset>>	asset;
	};
	struct Cutting
	{
		bool gender;
		std::string name;
	};
	struct Slots
	{
		std::map<std::string, const std::shared_ptr<Slot>> slot;
	};

	ItemModel() = default;
	~ItemModel() = default;

public:
	auto PushSlotItem(const std::shared_ptr<Slot>& item)->void;
	auto PushCuttingItem(const std::shared_ptr<Cutting>& item)->void;

	auto SetId(int32_t id)->void;
	auto GetId() const ->int32_t;

	auto SetDesc(const std::string& desc)->void;
	auto GetDesc() const ->std::string;

	auto GetSlots()const->const Slots&;

private:
	int32_t					_id{};
	std::string				_desc;
	Slots					_slots;
	std::vector<std::shared_ptr<Cutting>>	_cuttings;
};

