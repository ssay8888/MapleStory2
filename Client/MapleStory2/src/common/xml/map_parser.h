#pragma once
class MapParser
{
public:
	struct Property
	{
		std::map<std::string, _float3> property;
	};
	struct MapEntity
	{
		std::string id;
		std::string modelName;
		std::string name;
		std::string iterations;
		std::vector<Property> propertise;
	};
	static auto MapModelNameListExport()->std::list<std::string>;
	static auto MapParsing()->std::vector<MapEntity>;
};

