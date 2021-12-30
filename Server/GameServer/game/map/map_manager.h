#pragma once
class GameCharacter;
class MapInstance;

class MapManager
{
private:
	MapManager();

public:
	struct SpawnPoint
	{
		_float3 position;
	};
	static auto GetInstance()->MapManager&
	{
		static MapManager instance;
		return instance;
	}

public:
	auto FindMapInstance(int32_t mapId)->std::shared_ptr<MapInstance>;

private:
	auto LoadMapInstance()->void;
	auto MapSpawnPointParsing(std::wstring name)->std::vector<_float3>;

private:
	std::map<int32_t, std::shared_ptr<MapInstance>> _maps;
};

