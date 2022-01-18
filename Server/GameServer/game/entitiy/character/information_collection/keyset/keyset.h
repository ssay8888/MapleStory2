#pragma once
#include "game/entitiy/character/information_collection/base_info.h"

class Keyset final : public BaseInfo
{
public:
	explicit Keyset(int64_t characterId);

	~Keyset() override = default;
	
public:
	auto PushItem(int32_t index, int32_t value)->void;
	auto RemoveItem(int32_t index)->void;
	auto FindItem(int32_t index)->int32_t;
	auto ChangeItem(int32_t index, int32_t value)->void;

	auto GetAllItems()const->std::map<int32_t, int32_t>;

	auto KeyMapListToXml()->std::wstring;

public:
	static auto Create(int64_t characterId)->std::shared_ptr<Keyset>;

private:
	auto NativeConstruct()->HRESULT;

private:
	std::map<int32_t, int32_t> _key_map;
};

