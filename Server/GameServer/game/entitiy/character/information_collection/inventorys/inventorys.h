#pragma once
#include "inventory.h"
#include "game/entitiy/character/information_collection/base_info.h"

class Inventory;

class Inventorys : public BaseInfo
{
public:
	explicit Inventorys(int64_t characterId);
	virtual ~Inventorys() = default;

public:
	auto PushItem(Protocol::kInventoryType type, int32_t position, int32_t itemId)->bool;
	auto FindItem(Protocol::kInventoryType type, int32_t position)->int32_t;
	auto RemoveItem(Protocol::kInventoryType type, int32_t position)->bool;
	auto AllItems()->std::vector<std::pair<int32_t, int32_t>>;
	auto AllItems(Protocol::kInventoryType type)->std::vector<std::pair<int32_t, int32_t>>;

public:
	auto ItemListToXml()const->std::wstring;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<Inventorys>;

private:
	auto NativeConstruct()->HRESULT;

private:
	std::map<Protocol::kInventoryType, std::shared_ptr<Inventory>> _inventory;
};

