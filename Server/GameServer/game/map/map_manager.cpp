#include "game_server_pch.h"
#include "map_manager.h"

#include <pugixml.hpp>
#include <sstream>
#include "map_instance.h"
#include "data_reader/data_reader_manager.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "map_object/map_object.h"
#include "src/utility/components/transform/transform.h"

MapManager::MapManager()
{
	LoadMapInstance();
}

auto MapManager::FindMapInstance(const int32_t mapId) -> std::shared_ptr<MapInstance>
{
	const auto iterator = _maps.find(mapId);
	if (iterator != _maps.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto MapManager::AllMapInstance() const -> std::vector<std::shared_ptr<MapInstance>>
{
	return _map_instances;
}

auto MapManager::LoadMapInstance() -> void
{
	const auto fieldDatas = DataReaderManager::GetInstance().AllFieldData();

	for (const auto& fieldDataPair : fieldDatas)
	{
		if (const auto spawnPoint = MapSpawnPointParsing(fieldDataPair.second->environment.name); !spawnPoint.empty())
		{
			const auto mapObjects = MapPasing(fieldDataPair.second->environment.name);
			const auto regions = MapResionPointParsing(fieldDataPair.second->environment.name);
			auto mapInstance = std::make_shared<MapInstance>(fieldDataPair.first);
			mapInstance->SetSpawnPoint(spawnPoint);
			mapInstance->SetRegionPoint(regions);
			mapInstance->AddObjects(mapObjects);
			_maps.emplace(fieldDataPair.first, mapInstance);
			_map_instances.push_back(mapInstance);
		}
	}
}

auto MapManager::MapPasing(const std::wstring& name) -> std::vector<MapEntity>
{
	pugi::xml_document doc;
	wchar_t xmlPath[100] = L"";
	swprintf_s(xmlPath, L"../../Binary/Resources/MapData/%s.xblock", name.c_str());
	auto err = doc.load_file(xmlPath);
	std::list<std::string> overlap_node_items;
	std::list<std::string> real_node_items;
	std::vector<MapEntity> entities;
	if (err.status == pugi::status_ok)
	{
		auto data = doc.select_nodes("game/entitySet/entity");

		for (auto& entity : data)
		{
			MapEntity map;
			map.id = entity.node().attribute("id").value();
			auto modelName = std::string(entity.node().attribute("modelName").value());
			auto underBar = *modelName.rbegin();

			if (underBar == '_')
			{
				map.modelName = modelName.substr(0, modelName.length() - 1);
			}
			else
			{
				map.modelName = std::string(entity.node().attribute("modelName").value());
			}
			map.name = entity.node().attribute("name").value();
			map.iterations = entity.node().attribute("iterations").value();

			if (map.name.find("]") != std::string::npos)
			{
				for (auto& property : entity.node())
				{
					if (!strcmp(property.attribute("name").value(), "Position") ||
						!strcmp(property.attribute("name").value(), "Rotation") ||
						!strcmp(property.attribute("name").value(), "portal"))
					{
						for (auto& value : property)
						{
							std::string posValue = value.attribute("value").value();
							std::istringstream ss(posValue);
							std::string temp;
							std::vector<float> values;
							while (std::getline(ss, temp, ','))
							{
								values.push_back(std::stof(temp));
							}
							if (values.size() >= 3)
							{
								map.propertise.emplace(property.attribute("name").value(), _float3(values[0], values[2], values[1]));
							}
						}
					}
				}
				entities.push_back(map);
			}
		}
	}
	return entities;
}

auto MapManager::MapSpawnPointParsing(const std::wstring name) -> std::vector<_float3>
{
	pugi::xml_document doc;
	wchar_t xmlPath[100] = L"";
	swprintf_s(xmlPath, L"../../Binary/Resources/MapData/%s.xblock", name.c_str());
	auto err = doc.load_file(xmlPath);
	std::vector<_float3> positions;
	if (err.status == pugi::status_ok)
	{
		const auto data = doc.select_nodes("game/entitySet/entity[@modelName='SpawnPointPC']/property[@name='Position']/set");

		for (auto& entity : data)
		{
			_float3 position;
			auto modelName = std::string(entity.node().attribute("modelName").value());
			auto posValue = entity.node().attribute("value").value();
			std::istringstream ss(posValue);
			std::string temp;
			std::vector<float> values;
			while (std::getline(ss, temp, ','))
			{
				values.push_back(std::stof(temp));
			}
			if (values.size() >= 3)
			{
				positions.push_back(_float3(values[0] / 150.f * 0.58f, values[2] / 150.f * 0.58f, values[1] / 150.f * 0.58f));
			}
		}
	}
	return positions;
}

auto MapManager::MapResionPointParsing(std::wstring name) -> std::vector<std::shared_ptr<SpawnPoint>>
{
	pugi::xml_document doc;
	wchar_t xmlPath[100] = L"";
	swprintf_s(xmlPath, L"../../Binary/Resources/MapData/%s.xblock", name.c_str());
	auto err = doc.load_file(xmlPath);
	std::vector<std::shared_ptr<SpawnPoint>> positions;
	if (err.status == pugi::status_ok)
	{
		const auto data = doc.select_nodes("game/entitySet/entity");

		for (auto& entity : data)
		{
			_float3 position;
			auto modelName = std::string(entity.node().attribute("modelName").value());
			if (modelName.find("MS2RegionSpawn") != std::string::npos)
			{
				auto posValue = entity.node().attribute("value").value();
				std::istringstream ss(posValue);

				auto spawnPoint = std::make_shared<SpawnPoint>();
				for (auto& property : entity.node())
				{
					if (!strcmp(property.attribute("name").value(), "Position"))
					{
						for (auto& value : property)
						{
							std::string posValue = value.attribute("value").value();
							std::istringstream ss(posValue);
							std::string temp;
							std::vector<float> values;
							while (std::getline(ss, temp, ','))
							{
								values.push_back(std::stof(temp));
							}
							if (values.size() >= 3)
							{
								spawnPoint->SetPosition(_float3(values[0] / 150.f * 0.58f, values[2] / 150.f * 0.58f, values[1] / 150.f * 0.58f));
							}
						}
					} else if (!strcmp(property.attribute("name").value(), "spawnMonster"))
					{
						for (auto& value : property)
						{
							int32_t spawnNpcId = std::stoi(value.attribute("value").value());
							spawnPoint->SetSpawnNpcId(spawnNpcId);
						}
					}
				}
				positions.push_back(spawnPoint);
			}
		}
	}
	return positions;
}