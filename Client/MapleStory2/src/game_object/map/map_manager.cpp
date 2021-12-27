#include "c_pch.h"
#include "map_manager.h"

#include "map_instance.h"
#include "src/common/xml/map_parser.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/game_objects/manager/object_manager.h"

auto MapManager::LoadMapInstance(kScene scene) -> void
{
	auto mapData = MapParser::MapParsing("02000003_ad");

	auto mapInstance = std::make_shared<MapInstance>(GraphicDevice::GetInstance().GetDevice());

	for (auto& entity : mapData)
	{
		entity.scene = scene;
		mapInstance->AddMapObject(entity);
	}
	ObjectManager::GetInstance().AddGameObject(scene, L"MapLayer_", mapInstance);
	_maps.emplace(L"02000003_ad", mapInstance);
}

auto MapManager::LoadCharacterInstance(kScene scene) -> void
{
	auto mapData = MapParser::CharacterSelectMapParsing();

	auto mapInstance = std::make_shared<MapInstance>(GraphicDevice::GetInstance().GetDevice());

	for (auto& entity : mapData)
	{
		entity.scene = scene;
		mapInstance->AddMapObject(entity);
	}
	ObjectManager::GetInstance().AddGameObject(scene, L"MapLayer_", mapInstance);
	_maps.emplace(L"CharacterSelect", mapInstance);
}

auto MapManager::FindMapInstance(const std::wstring& mapid) -> std::shared_ptr<MapInstance>
{
	const auto iterator = _maps.find(mapid);
	if (iterator == _maps.end())
	{
		return nullptr;
	}
	return iterator->second;
}
