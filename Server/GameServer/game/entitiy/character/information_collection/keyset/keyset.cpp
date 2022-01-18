#include "game_server_pch.h"
#include "keyset.h"

#include <pugixml.hpp>
#include <sstream>

#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"

Keyset::Keyset(const int64_t characterId):
	BaseInfo(characterId)
{
}

auto Keyset::PushItem(int32_t index, int32_t value)->void
{
	_key_map.emplace(index, value);
}

auto Keyset::RemoveItem(const int32_t index) -> void
{
	_key_map.erase(index);
}

auto Keyset::FindItem(int32_t index) -> int32_t
{
	const auto iterator = _key_map.find(index);
	if (iterator != _key_map.end())
	{
		return iterator->second;
	}
	return -1;
}

auto Keyset::ChangeItem(const int32_t index, const int32_t value) -> void
{
	const auto iterator = _key_map.find(index);
	if (iterator != _key_map.end())
	{
		iterator->second = value;
	}
}

auto Keyset::GetAllItems() const -> std::map<int32_t, int32_t>
{
	return _key_map;
}

auto Keyset::KeyMapListToXml() -> std::wstring
{
	using namespace pugi;
	xml_document doc;
	xml_node root = doc.append_child("ms2");

	auto items = root.append_child("keylist");
	for (const auto& [index, keyvalue] : _key_map)
	{
		auto item = items.append_child("keymap");
		item.append_attribute("keyindex").set_value(index);
		item.append_attribute("keyvalue").set_value(keyvalue);
	}
	std::wstringstream xml;
	doc.save(xml);
	return xml.str();
}

auto Keyset::Create(int64_t characterId) -> std::shared_ptr<Keyset>
{
	auto instance = MakeShared<Keyset>(characterId);
	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}
	return instance;
}

auto Keyset::NativeConstruct() -> HRESULT
{
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 2> bind(*con, L"{CALL dbo.spLoadKeymap(?)}");
		bind.BindParam(0, _character_id);

		int32_t keyIndex = 0;
		bind.BindCol(0, keyIndex);
		int32_t keyValue = 0;
		bind.BindCol(1, keyValue);

		if (bind.Execute())
		{
			do
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (bind.Fetch())
					{
						_key_map.emplace(keyIndex, keyValue);
					}
				}
			} while (bind.SqlMoreResults() != SQL_NO_DATA);
		}

	}
	return S_OK;
}
