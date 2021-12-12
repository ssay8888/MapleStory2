#include "common_pch.h"
#include "data_reader_manager.h"

#include <iostream>
#include <pugixml.hpp>

#include "files_manager/items/item_model.h"
#include "string_utils/string_utils.h"

using namespace pugi;

auto DataReaderManager::Init() -> void
{
	LoadItemModel("../../Binary/Resources/Xml/itemmodel/150.xml");
	LoadAnimationInfo();
}

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
			}
			_item_model.emplace(itemModel->GetId(), itemModel);
		}
	}
	std::cout << "";
}

auto DataReaderManager::FindItemModel(const int32_t itemId) -> std::shared_ptr<ItemModel>
{
	if (const auto iterator = _item_model.find(itemId); iterator != _item_model.end())
	{
		return iterator->second;
	}
	return nullptr;

}

#pragma region AnimationInfo

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
	int a = 0 ;
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

#pragma endregion
