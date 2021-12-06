#include "c_pch.h"
#include "map_manager.h"

#include "map_instance.h"
#include "src/common/xml/map_parser.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/game_objects/manager/object_manager.h"

void MapManager::LoadMapInstance(kScene scene)
{
	auto mapData = MapParser::MapParsing();

	auto mapInstance = std::make_shared<MapInstance>(GraphicDevice::GetInstance().GetDevice());

	for (auto& entity : mapData)
	{
		entity.scene = scene;
		mapInstance->AddMapObject(entity);
	}
	ObjectManager::GetInstance().AddGameObject(scene, L"MapLayer_", mapInstance);
}

void MapManager::LoadCharacterInstance(kScene scene)
{
	auto mapData = MapParser::CharacterSelectMapParsing();

	auto mapInstance = std::make_shared<MapInstance>(GraphicDevice::GetInstance().GetDevice());

	for (auto& entity : mapData)
	{
		entity.scene = scene;
		mapInstance->AddMapObject(entity);
	}
	ObjectManager::GetInstance().AddGameObject(scene, L"MapLayer_", mapInstance);
}
