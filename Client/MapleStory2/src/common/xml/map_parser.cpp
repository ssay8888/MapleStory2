#include "c_pch.h"
#include "map_parser.h"
#include <pugixml.hpp>
#include <sstream>

using namespace pugi;

auto MapParser::MapModelNameListExport()->std::list<std::string>
{
	xml_document doc;
	char xmlPath[100] = "../../Binary/Resources/MapData/02000003_ad.xblock";
	auto err = doc.load_file(xmlPath);
	std::list<std::string> overlap_node_items;
	std::list<std::string> real_node_items;
	if (err.status == status_ok)
	{
		auto data = doc.select_nodes("game/entitySet/entity");

		for (auto& items : data)
		{
			overlap_node_items.push_back(items.node().attribute("modelName").value());
			//std::cout << items.node().attribute("modelName").value() << std::endl;
		}
	}
	for (auto iter = overlap_node_items.begin(); iter != overlap_node_items.end(); ++iter)
	{
		bool check = false;
		for (auto real = real_node_items.begin(); real != real_node_items.end(); ++real)
		{
			if (*real == *iter)
			{
				check = true;
			}
		}
		if (!check)
		{
			auto underBar = *(*iter).rbegin();
			if (underBar == '_')
			{
				real_node_items.push_back((*iter).substr(0, (*iter).length() - 1));
			}
			else
			{
				real_node_items.push_back(*iter);
			}
		}
	}
	//real_node_items.sort();

	return real_node_items;
}

auto MapParser::MapParsing()->std::vector<MapEntity>
{
	xml_document doc;
	//L"Client\\Character\\00012000.img.xml"
	char xmlPath[100] = "../../Binary/Resources/MapData/02000003_ad.xblock";
	//snprintf(xmlPath, 100, , size);
	auto err = doc.load_file(xmlPath);
	std::list<std::string> overlap_node_items;
	std::list<std::string> real_node_items;
	std::vector<MapEntity> entities;
	if (err.status == status_ok)
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

			for (auto& property : entity.node())
			{
				Property prop;

				if (!strcmp(property.attribute("name").value(), "Position") || 
					!strcmp(property.attribute("name").value(), "Rotation"))
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
							prop.property.emplace(property.attribute("name").value(), _float3(values[0], values[2], values[1]));
						}
					}
				}
				map.propertise.push_back(prop);
			}
			entities.push_back(map);
		}
	}
	std::cout << entities.size() << std::endl;
	return entities;
}
