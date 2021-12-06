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

	void LoadMapInstance(kScene scene);
	void LoadCharacterInstance(kScene scene);

private:
	std::map<std::wstring, std::shared_ptr<MapInstance>> _maps;
};

