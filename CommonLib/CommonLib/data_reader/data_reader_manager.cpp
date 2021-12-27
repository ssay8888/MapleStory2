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
	FaceLoader(device);
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

			_animations_info.emplace(info->animation_name, info);
		}
	}
}

auto DataReaderManager::FindAnimationInfo(const std::string& name) -> std::shared_ptr<AnimationInfo>
{
	auto iterator = _animations_info.find(name);

	if (iterator != _animations_info.end())
	{
		return iterator->second;
	}

	return nullptr;
}

auto DataReaderManager::AllAnimationName() -> std::vector<std::shared_ptr<AnimationInfo>>
{
	std::vector<std::shared_ptr<AnimationInfo>>	infos;
	for (auto& ani : _animations_info)
	{
		infos.push_back(ani.second);
	}

	return infos;
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

#pragma endregion
