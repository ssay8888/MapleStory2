#include "common_pch.h"
#include "data_reader_manager.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <iostream>
#include <pugixml.hpp>
#include <boost/filesystem/path.hpp>
#include <wrl/client.h>

#include "files_manager/file_manager.h"
#include "files_manager/items/item_model.h"
#include "string_utils/string_utils.h"

using namespace pugi;

auto DataReaderManager::Init(const Microsoft::WRL::ComPtr<IDirect3DDevice9> device) -> void
{
	LoadItemModel("../../Binary/Resources/Xml/itemmodel/114.xml");
	LoadItemModel("../../Binary/Resources/Xml/itemmodel/115.xml");
	LoadItemModel("../../Binary/Resources/Xml/itemmodel/117.xml");
	LoadItemModel("../../Binary/Resources/Xml/itemmodel/150.xml");
	LoadAnimationInfo();
	LoadCreateCharacterItemInfo();
	LoadFieldData();
	LoadMonsterInfo();
	if (device)
	{
		FaceLoader(device);
	}
}

#pragma region ItemInfoMethod
auto DataReaderManager::LoadItemModel(const std::string& path) -> void
{
	xml_document doc;
	const auto err = doc.load_file(path.c_str());

	if (err.status == status_ok)
	{
		const auto models = doc.select_nodes("ms2/ItemModel");

		
		for (auto& model : models)
		{
			const auto itemModel = std::make_shared<ItemModel>();
			itemModel->SetId(std::stoi(model.node().attribute("id").value()));
			itemModel->SetDesc(model.node().attribute("desc").value());
			for (auto& items : model.node())
			{

				if (!strcmp(items.name(), "slots"))
				{
					for (auto& slot : items)
					{
						const auto slotItem = std::make_shared<ItemModel::Slot>();
						slotItem->name = slot.attribute("name").value();
						for (auto& asset : slot)
						{
							const auto assetItem = std::make_shared<ItemModel::Asset>();
							assetItem->name = asset.attribute("name").value();
							assetItem->self_node = asset.attribute("selfnode").value();
							assetItem->target_node = asset.attribute("targetnode").value();
							assetItem->gender = std::stoi((asset.attribute("gender") ? asset.attribute("gender").value() : "0"));
							for (auto& dummy : asset)
							{
								const auto dummyItem = std::make_shared<ItemModel::Dummy>();
								dummyItem->gender = std::stoi((dummy.attribute("gender") ? dummy.attribute("gender").value() : "0"));
								dummyItem->target_node = dummy.attribute("targetnode").value();
								dummyItem->rotation = dummy.attribute("rotation").value();
								dummyItem->translation = dummy.attribute("translation").value();
								assetItem->dummies.push_back(dummyItem);
							}
							slotItem->asset.push_back(assetItem);
						}
						itemModel->PushSlotItem(slotItem);
					}
				}
				else if (!strcmp(items.name(), "cutting"))
				{
					for (auto& cutting : items)
					{
						const auto cuttingItem = std::make_shared<ItemModel::Cutting>();

						if (cutting.attribute("gender"))
						{
							cuttingItem->gender = std::stoi(cutting.attribute("gender").value());
						}
						cuttingItem->name = cutting.attribute("name").value();
						itemModel->PushCuttingItem(cuttingItem);
					}
				}
			}
			_item_model.emplace(itemModel->GetId(), itemModel);
		}
	}
}

auto DataReaderManager::FindItemModel(const int32_t itemId) -> std::shared_ptr<ItemModel>
{
	if (const auto iterator = _item_model.find(itemId); iterator != _item_model.end())
	{
		return iterator->second;
	}
	return nullptr;

}

auto DataReaderManager::AllItemModel() const -> const std::map<int32_t, std::shared_ptr<ItemModel>>&
{
	return _item_model;
}

#pragma endregion

#pragma region AnimationLoader

auto DataReaderManager::LoadAnimationInfo()->void
{
	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/Xml/animationinfo/animationInfo.xml");

	if (err.status == status_ok)
	{
		const auto models = doc.select_nodes("ms2/model");

		for (auto& model : models)
		{
			auto info = std::make_shared<AnimationInfo>();

			info->animation_name = model.node().attribute("animation_name").value();
			info->target_node = model.node().attribute("targetnode").value();

			_animations_info.push_back(info);
		}
	}
}

auto DataReaderManager::FindAnimationInfo(const std::string& name) -> std::shared_ptr<AnimationInfo>
{
	/*auto iterator = _animations_info.find(name);

	if (iterator != _animations_info.end())
	{
		return iterator->second;
	}*/

	return nullptr;
}

auto DataReaderManager::AllAnimationName() const -> std::vector<std::shared_ptr<AnimationInfo>>
{
	return _animations_info;
}

auto DataReaderManager::FaceLoader(const Microsoft::WRL::ComPtr<IDirect3DDevice9> device) -> void
{

	auto dirs = FileManager::GetDirFileName(L"../../Binary/Resources/Model/texture/face/");

	for (auto& dir : dirs)
	{
		boost::filesystem::path path(dir);
		auto face_id = std::stoi(path.filename().c_str());
		auto files = FileManager::GetDirFileName(dir.append(L"/"));
		auto info = std::make_shared<FaceInfo>();
		info->item_id = face_id;
		for (auto& file : files)
		{
			if (device)
			{
				Microsoft::WRL::ComPtr<IDirect3DTexture9> diffuse;
				if (FAILED(D3DXCreateTextureFromFile(device.Get(), file.c_str(), &diffuse)))
				{
					std::cout << "Face 로드 실패. 얼굴 코드 : " << face_id << std::endl;
					continue;
				}
				info->diffuse_map.push_back(diffuse);


				std::wstring iconPath(TEXT("../../Binary/Resources/Image/item/icon/customize/%08d.png"));
				wchar_t name[MAX_PATH];
				swprintf_s(name, MAX_PATH, iconPath.c_str(), info->item_id);
				if (FAILED(D3DXCreateTextureFromFile(device.Get(), name, &diffuse)))
				{
				std::cout << "Face Icon 로드 실패. 얼굴 코드 : " << face_id << std::endl;
					continue;
				}
				info->icon_diffuse_map = diffuse;
			}

			info->files.push_back(file);
		}
		_face_info.emplace(info->item_id, info);
	}
}

auto DataReaderManager::FindFace(const int32_t faceId) -> std::shared_ptr<FaceInfo>
{
	auto iter = _face_info.find(faceId);

	if (iter != _face_info.end())
	{
		return iter->second;
	}
	return nullptr;
}

auto DataReaderManager::LoadCreateCharacterItemInfo() -> void
{
	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/Xml/createItem.xml");

	if (err.status == status_ok)
	{
		const auto itemList = doc.select_nodes("ms2/itemList");

		for (auto& items : itemList)
		{
			auto info = std::make_shared<CreateCharacterItemInfo>();
			info->title_name = StringUtils::ConvertCtoW(items.node().attribute("name").value());
  			for (auto& itemNode : items.node())
			{
				auto item = std::make_shared<Item>();
				item->item_id = std::stoi(itemNode.attribute("itemid").value());
				item->gender = std::stoi(itemNode.attribute("gender").value());
				item->type = std::stoi(itemNode.attribute("type").value());
				info->items.push_back(item);
			}
			_create_item_info.push_back(info);
		}
	}
}

auto DataReaderManager::GetCreateItemInfo() const -> std::vector<std::shared_ptr<CreateCharacterItemInfo>>
{
	return _create_item_info;
}

auto DataReaderManager::LoadFieldData() -> void
{
	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/MapData/fielddata.xml");

	if (err.status == status_ok)
	{
		const auto fieldList = doc.select_nodes("ms2/fieldData");

		for (auto field : fieldList)
		{
			auto fieldData = std::make_shared<FieldData>();
			fieldData->id = std::stoi(field.node().attribute("id").value());

			for (auto environmentNode : field.node())
			{
				fieldData->environment.feature = StringUtils::ConvertCtoW(environmentNode.attribute("feature").value());
				for (auto dataNode : environmentNode)
				{
					std::string data = dataNode.name();

					if (data == "xblock")
					{
						fieldData->environment.name = StringUtils::ConvertCtoW(dataNode.attribute("name").value());
					}
					else if (data == "bgm")
					{
						fieldData->environment.bgm.id = std::stoi(dataNode.attribute("id").value());
						if (dataNode.attribute("param"))
						{
							fieldData->environment.bgm.param = std::stoi(dataNode.attribute("param").value());
						}
					}
				}
			}
			_field_data.emplace(fieldData->id, fieldData);
		}
	}
}

auto DataReaderManager::AllFieldData() const -> std::map<int32_t, std::shared_ptr<FieldData>>
{
	return _field_data;
}
auto DataReaderManager::LoadMonsterInfo() -> void
{
	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/xml/npcdata/210.xml");

	if (err.status == status_ok)
	{
		const auto fieldList = doc.select_nodes("ms2/npc");

		for (auto npcNode : fieldList)
		{
			auto monsterInfo = std::make_shared<MonsterInfo>();
			monsterInfo->id = std::stoi(npcNode.node().attribute("id").value());

			for (auto enviromnment : npcNode.node())
			{
				for (auto node : enviromnment)
				{
					std::string nodeName = node.name();
					if (nodeName == "model")
					{
						monsterInfo->model.kfm = StringUtils::ConvertCtoW(node.attribute("kfm").value());
						if (strcmp(node.attribute("walkSpeed").value(), ""))
						{
							monsterInfo->model.walk_speed = std::stoi(node.attribute("walkSpeed").value());
						}
						if (strcmp(node.attribute("runSpeed").value(), ""))
						{
							monsterInfo->model.run_speed = std::stoi(node.attribute("runSpeed").value());
						}
					}
					else if (nodeName == "stat")
					{
						if (strcmp(node.attribute("hp").value(), ""))
						{
							monsterInfo->stat.hp = std::stoi(node.attribute("hp").value());
						}
						if (strcmp(node.attribute("asp").value(), ""))
						{
							monsterInfo->stat.asp = std::stoi(node.attribute("asp").value());
						}
						if (strcmp(node.attribute("msp").value(), ""))
						{
							monsterInfo->stat.msp = std::stoi(node.attribute("msp").value());
						}
						if (strcmp(node.attribute("atp").value(), ""))
						{
							monsterInfo->stat.atp = std::stoi(node.attribute("atp").value());
						}
						if (strcmp(node.attribute("evp").value(), ""))
						{
							monsterInfo->stat.evp = std::stoi(node.attribute("evp").value());
						}
						if (strcmp(node.attribute("cap").value(), ""))
						{
							monsterInfo->stat.cap = std::stoi(node.attribute("cap").value());
						}
						if (strcmp(node.attribute("cad").value(), ""))
						{
							monsterInfo->stat.cad = std::stoi(node.attribute("cad").value());
						}
						if (strcmp(node.attribute("car").value(), ""))
						{
							monsterInfo->stat.car = std::stoi(node.attribute("car").value());
						}
						if (strcmp(node.attribute("ndd").value(), ""))
						{
							monsterInfo->stat.ndd = std::stoi(node.attribute("ndd").value());
						}
						if (strcmp(node.attribute("pap").value(), ""))
						{
							monsterInfo->stat.pap = std::stoi(node.attribute("pap").value());
						}
						if (strcmp(node.attribute("map").value(), ""))
						{
							monsterInfo->stat.map = std::stoi(node.attribute("map").value());
						}
						if (strcmp(node.attribute("par").value(), ""))
						{
							monsterInfo->stat.par = std::stoi(node.attribute("par").value());
						}
						if (strcmp(node.attribute("par").value(), ""))
						{
							monsterInfo->stat.mar = std::stoi(node.attribute("mar").value());
						}
					}
					else if (nodeName == "distance")
					{
						if (strcmp(node.attribute("sight").value(), ""))
						{
							monsterInfo->distance.sight = std::stoi(node.attribute("sight").value());
						}
						if (strcmp(node.attribute("sightHeightUP").value(), ""))
						{
							monsterInfo->distance.sight_height_up = std::stoi(node.attribute("sightHeightUP").value());
						}
						if (strcmp(node.attribute("sightHeightDown").value(), ""))
						{
							monsterInfo->distance.sight_height_down = std::stoi(node.attribute("sightHeightDown").value());
						}
					}
					else if (nodeName == "collision")
					{
						monsterInfo->collision.width = std::stoi(node.attribute("width").value());
						monsterInfo->collision.height = std::stoi(node.attribute("height").value());
						monsterInfo->collision.depth = std::stoi(node.attribute("depth").value());
					}
				}
			}
			_monster_info.emplace(monsterInfo->id, monsterInfo);
		}
	}
}

#pragma endregion
