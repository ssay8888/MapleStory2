#pragma once
class SpawnPoint;
class MapObject;
class GameCharacter;
class MapInstance;

class MapManager
{
private:
	MapManager();

public:
	struct MapEntity
	{
		std::string id;
		std::string modelName;
		std::string name;
		std::string iterations;
		std::map<std::string, _float3> propertise;
	};

	static auto GetInstance()->MapManager&
	{
		static MapManager instance;
		return instance;
	}

public:
	auto FindMapInstance(int32_t mapId)->std::shared_ptr<MapInstance>;
	auto AllMapInstance() const ->std::vector<std::shared_ptr<MapInstance>>;
private:
	auto LoadMapInstance()->void;
	auto MapPasing(const std::wstring& name)->std::vector<MapEntity>;
	auto MapSpawnPointParsing(std::wstring name)->std::vector<_float3>;
	auto MapResionPointParsing(std::wstring name)->std::vector<std::shared_ptr<SpawnPoint>>;

private:
	std::map<int32_t, std::shared_ptr<MapInstance>> _maps;
	std::vector<std::shared_ptr<MapInstance>> _map_instances;
};

