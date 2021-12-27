#pragma once

class MapInstance;

class MapManager
{
public:
	MapManager() = default;
	~MapManager() = default;
	static auto GetInstance()->MapManager&
	{
		static MapManager instance;
		return instance;
	}

	auto LoadMapInstance(kScene scene)->void;
	auto LoadCharacterInstance(kScene scene)->void;

	auto FindMapInstance(const std::wstring& mapid)->std::shared_ptr<MapInstance>;



private:
	std::map<std::wstring, std::shared_ptr<MapInstance>> _maps;
};

