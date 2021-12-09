#include "common_pch.h"
#include "data_reader_manager.h"

#include <iostream>
#include <pugixml.hpp>

#include "files_manager/items/item_model.h"

using namespace pugi;
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
