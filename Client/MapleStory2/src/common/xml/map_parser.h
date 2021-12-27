#pragma once
class MapParser
{
public:
	struct MapEntity
	{
		std::string id;
		std::string modelName;
		std::string name;
		std::string iterations;
		std::map<std::string, _float3> propertise;
		kScene scene;
	};
	static auto MapModelNameListExport(const std::string name)->std::list<std::string>;
	static auto MapParsing(const std::string name)->std::vector<MapEntity>;


	static auto CharacterSelectMapParsing()->std::vector<MapEntity>;
};

