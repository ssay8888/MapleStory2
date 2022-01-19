#include "common_pch.h"
#include "data_reader_manager.h"

#include <algorithm>
#include <d3d9.h>
#include <d3dx9tex.h>
#include <iostream>
#include <pugixml.hpp>
#include <sstream>
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
	LoadItemInfo("../../Binary/Resources/Xml/itemdata/114.xml");
	LoadItemInfo("../../Binary/Resources/Xml/itemdata/115.xml");
	LoadItemInfo("../../Binary/Resources/Xml/itemdata/117.xml");
	LoadItemInfo("../../Binary/Resources/Xml/itemdata/200.xml");
	LoadAnimationInfo();
	LoadItemOptionConstant();
	LoadCreateCharacterItemInfo();
	LoadFieldData();
	LoadMonsterInfo();
	LoadAniKeyText();
	LoadAniKeyPlayerText();
	LoadSkillData();
	LoadMotionEffect();
	LoadTagEffect();
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

auto DataReaderManager::LoadItemInfo(std::string path) -> void
{
	xml_document doc;
	const auto err = doc.load_file(path.c_str());

	if (err.status == status_ok)
	{
		const auto nodeItems = doc.select_nodes("ms2/item");

		for (auto nodeItem : nodeItems)
		{
			const auto item = std::make_shared<ItemInfo>();
			item->id = std::stoi(nodeItem.node().attribute("id").value());
			for (auto environment : nodeItem.node())
			{
				for (auto info : environment)
				{
					if (std::string(info.name()) == "property")
					{
						item->property.slot_max = info.attribute("slotMax").empty() ? 1 : std::stoi(info.attribute("slotMax").value());
						item->property.slot_icon = info.attribute("slotIcon").empty() ? L"" : StringUtils::ConvertCtoW(info.attribute("slotIcon").value());
						item->property.icon_code = info.attribute("iconCode").empty() ? -1 : std::stoi(info.attribute("iconCode").value());
						item->property.category = StringUtils::ConvertCtoW(info.attribute("category").value());
					}
					else if (std::string(info.name()) == "skill")
					{
						item->skill_id = info.attribute("skillID").empty() ? -1 : std::stoi(info.attribute("skillID").value());
					}
					else if (std::string(info.name()) == "option")
					{
						item->constant_id = info.attribute("constantID").empty() ? -1 : std::stoi(info.attribute("constantID").value());
					}
				}
			}
			_item_info.emplace(item->id, item);
		}
	}
}

auto DataReaderManager::AllItemInfo() const -> std::map<int32_t, std::shared_ptr<ItemInfo>>
{
	return _item_info;
}

auto DataReaderManager::FindItemInfo(int32_t itemId) -> std::shared_ptr<ItemInfo>
{
	const auto& iterator = _item_info.find(itemId);
	if (iterator != _item_info.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto DataReaderManager::LoadItemOptionConstant() -> void
{
	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/Xml/itemoption/itemoptionconstant.xml");

	if (err.status == status_ok)
	{
		const auto options = doc.select_nodes("ms2/option");

		for (auto optionNode : options)
		{
			const auto option = std::make_shared<Option>();
			option->code = std::stoi(optionNode.node().attribute("code").value());
			for (auto rank : optionNode.node())
			{
				for (auto v : rank)
				{
					auto rank = std::make_shared<Option::Rank>();
					rank->value = std::stoi(v.attribute("value").value());
					rank->lua_index = v.attribute("luaIndex").empty() ? 0 : std::stoi(v.attribute("luaIndex").value());
					rank->name = StringUtils::ConvertCtoW(v.attribute("name").value());
					option->ranks.emplace(rank->name, rank);
				}
			}
			_item_options.emplace(option->code, option);
		}
	}
}

auto DataReaderManager::FindItemOptionConstant(int32_t optionId) -> std::shared_ptr<Option>
{
	const auto& iterator = _item_options.find(optionId);
	if (iterator != _item_options.end())
	{
		return iterator->second;
	}
	return nullptr;
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

	auto dirs = FileManager::GetDirs(L"../../Binary/Resources/Meshes/DynamicMesh/NpcData");
	if (err.status == status_ok)
	{
		for (auto dir : dirs)
		{
			char_t xmlPath[100] = "";
			sprintf_s(xmlPath, "ms2/npc[@id='%d']", std::stoi(FileManager::GetFileName(dir)));
			const auto fieldList = doc.select_nodes(xmlPath);

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
								monsterInfo->model.walk_speed = std::stoi(node.attribute("walkSpeed").value()) / 150.f * 0.58f;
							}
							if (strcmp(node.attribute("runSpeed").value(), ""))
							{
								monsterInfo->model.run_speed = std::stoi(node.attribute("runSpeed").value()) / 150.f * 0.58f;
							}
							if (strcmp(node.attribute("scale").value(), ""))
							{
								monsterInfo->model.scale = std::stof(node.attribute("scale").value()) / 150;
							}
							else
							{
								monsterInfo->model.scale = 0.01f;
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
								monsterInfo->distance.sight = std::stof(node.attribute("sight").value()) / 150.f * 0.58f;
							}
							if (strcmp(node.attribute("sightHeightUP").value(), ""))
							{
								monsterInfo->distance.sight_height_up = std::stof(node.attribute("sightHeightUP").value()) / 150.f * 0.58f;
							}
							if (strcmp(node.attribute("sightHeightDown").value(), ""))
							{
								monsterInfo->distance.sight_height_down = std::stof(node.attribute("sightHeightDown").value()) / 150.f * 0.58f;
							}
						}
						else if (nodeName == "collision")
						{
							monsterInfo->collision.width = std::stof(node.attribute("width").value()) / 150.f * 0.4f;
							monsterInfo->collision.height = std::stof(node.attribute("height").value()) / 150.f * 0.4f;
							monsterInfo->collision.depth = std::stof(node.attribute("depth").value()) / 150.f * 0.4f;
						}
						else if (nodeName == "skill")
						{
							auto skills = std::string(node.attribute("ids").value());
							std::istringstream ss(skills);
							std::string temp;
							std::vector<int32_t> values;
							while (std::getline(ss, temp, ','))
							{
								values.push_back(std::stoi(temp));
							}
							auto probs = std::string(node.attribute("probs").value());
							std::istringstream ss2(probs);
							std::vector<int32_t> values2;
							while (std::getline(ss2, temp, ','))
							{
								values2.push_back(std::stoi(temp));
							}

							for (auto i = 0; i < values.size(); ++i)  
							{
								monsterInfo->skills.emplace_back(values[i], values2.empty() ? 100 : values2[i]);
							}
						}
					}
				}
				_monster_info.emplace(monsterInfo->id, monsterInfo);
		}
		}
	}
}

auto DataReaderManager::AllMonsterInfo() -> std::vector<std::shared_ptr<MonsterInfo>>
{
	std::vector<std::shared_ptr<MonsterInfo>> infos;
	for (const auto& monster : _monster_info)
	{
		infos.push_back(monster.second);
	}
	return infos;
}

auto DataReaderManager::FindMonsterInfo(const int32_t id) -> std::shared_ptr<MonsterInfo>
{
	const auto iterator = _monster_info.find(id);
	if (iterator != _monster_info.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto DataReaderManager::LoadAniKeyText() -> void
{

	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/xml/anikeytext.xml");
	
	if (err.status == status_ok)
	{
		const auto kfmNode = doc.select_nodes("ms2ani/kfm");
		for (auto kfm : kfmNode)
		{
			auto name = StringUtils::ConvertCtoW(kfm.node().attribute("name").value());
			for (const auto monsterInfo : _monster_info)
			{
				auto kfmLower = monsterInfo.second->model.kfm;
				StringUtils::ToLower(kfmLower);
				if (name.find(kfmLower) != std::wstring::npos)
				{
					auto kfmObject = std::make_shared<Kfm>();
					kfmObject->name = StringUtils::ConvertCtoW(kfm.node().attribute("name").value());
					for (auto seqNode : kfm.node())
					{
						int key = std::stoi(seqNode.attribute("id").value());
						auto seq = std::make_shared<Seq>();
						seq->name = StringUtils::ConvertCtoW(seqNode.attribute("name").value());
						kfmObject->seqs.emplace(key, seq);
						for (auto keyNode : seqNode)
						{
							kfmObject->seqs[key]->key.emplace(StringUtils::ConvertCtoW(keyNode.attribute("name").value()),
								std::stof(keyNode.attribute("time").value()));
						}
					}
					_ani_key.emplace(monsterInfo.first, kfmObject);
				}
			}
		}
	}
}

auto DataReaderManager::LoadAniKeyPlayerText() -> void
{

	xml_document doc;
	const auto err = doc.load_file("../../Binary/Resources/xml/anikeytext.xml");

	if (err.status == status_ok)
	{
		const auto kfmNode = doc.select_nodes("ms2ani/kfm");
		for (auto kfm : kfmNode)
		{
			auto name = StringUtils::ConvertCtoW(kfm.node().attribute("name").value());
			if (name == L"male" || name == L"female")
			{
				auto kfmObject = std::make_shared<Kfm>();
				kfmObject->name = StringUtils::ConvertCtoW(kfm.node().attribute("name").value());
				for (auto seqNode : kfm.node())
				{
					int key = std::stoi(seqNode.attribute("id").value());
					auto seq = std::make_shared<Seq>();
					seq->name = StringUtils::ConvertCtoW(seqNode.attribute("name").value());
					kfmObject->seqs.emplace(key, seq);
					for (auto keyNode : seqNode)
					{
						kfmObject->seqs[key]->key.emplace(StringUtils::ConvertCtoW(keyNode.attribute("name").value()),
							std::stof(keyNode.attribute("time").value()));
					}
				}
				_ani_key_player.emplace(name == L"male" ? 0 : 1, kfmObject);
			}
		}
	}
}

auto DataReaderManager::FindAniKey(const int32_t npcId) -> std::shared_ptr<Kfm>
{
	const auto iterator = _ani_key.find(npcId);
	if (iterator != _ani_key.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto DataReaderManager::FindPlayerAniKey(const int32_t gender) -> std::shared_ptr<Kfm>
{
	const auto iterator = _ani_key_player.find(gender);
	if (iterator != _ani_key_player.end())
	{
		return iterator->second;
	}
	return nullptr;
}

#pragma endregion


#pragma region SkillData


auto DataReaderManager::LoadSkillData() -> void
{
	auto files = FileManager::GetDirFileName(L"../../Binary/Resources/Xml/skilldata/");

	for (auto file : files)
	{
		xml_document doc;
		const auto err = doc.load_file(StringUtils::ConvertWtoC(file.c_str()).c_str());
		if (err.status == status_ok)
		{
			const auto skillNodes = doc.select_nodes("ms2/skill");
			for (auto skillNode : skillNodes)
			{
				auto skill = std::make_shared<Skill>();
				skill->id = std::stoi(skillNode.node().attribute("id").value());
				skill->comment = skillNode.node().attribute("comment").value();

				for (auto elementNode : skillNode.node())
				{
					if (std::string(elementNode.name()) == "level")
					{
						skill->cooldown = elementNode.attribute("cooldown").empty() ? 0 : std::stoi(elementNode.attribute("cooldown").value());
						for (auto infoNodes : elementNode)
						{
							if (std::string(infoNodes.name()) == "detectProperty")
							{
								skill->detect.distance = infoNodes.attribute("distance").empty() ? 0 : std::stof(infoNodes.attribute("distance").value()) / 150.f * 0.58f;
							}
							else if (std::string(infoNodes.name()) == "motion")
							{
								auto motion = std::make_shared<Motion>();
								motion->sequence_name = StringUtils::ConvertCtoW(infoNodes.attribute("sequenceName").value());
								StringUtils::ToLower(motion->sequence_name);
								motion->motion_effect = StringUtils::ConvertCtoW(infoNodes.attribute("motionEffect").value());
								StringUtils::ToLower(motion->motion_effect);
								motion->str_tag_effects = StringUtils::ConvertCtoW(infoNodes.attribute("strTagEffects").value());
								StringUtils::ToLower(motion->str_tag_effects);
								motion->movedistance = infoNodes.attribute("movedistance").empty() ? 0 : std::stof(infoNodes.attribute("movedistance").value()) / 150.f * 0.58f;
								for (auto attackNode : infoNodes)
								{
									if (std::string(attackNode.name()) == "attack")
									{
										motion->attack.target_count = attackNode.attribute("targetCount").empty() ? 0 : std::stoi(attackNode.attribute("targetCount").value());
										for (auto attackElement : attackNode)
										{
											if (std::string(attackElement.name()) == "range")
											{
												motion->attack.range.distance = attackElement.attribute("distance").empty() ? 0 : std::stof(attackElement.attribute("distance").value()) / 150.f * 0.58f;
												motion->attack.range.height = attackElement.attribute("height").empty() ? 0 : std::stof(attackElement.attribute("height").value()) / 150.f * 0.58f;
												motion->attack.range.width = attackElement.attribute("width").empty() ? 0 : std::stof(attackElement.attribute("width").value()) / 150.f * 0.58f;
											}
											else if (std::string(attackElement.name()) == "damage")
											{
												motion->attack.damage.count = attackElement.attribute("count").empty() ? 1 : std::stoi(attackElement.attribute("count").value());
												motion->attack.damage.rate = attackElement.attribute("rate").empty() ? 0 : std::stof(attackElement.attribute("rate").value());
											}
										}
									}
								}
								skill->motions.push_back(motion);
							}
						}
					}
				}
				_skills.emplace(skill->id, skill);
			}
		}
	}
}

auto DataReaderManager::FindSkillData(const int32_t skillId) -> std::shared_ptr<Skill>
{
	const auto iterator = _skills.find(skillId);
	if (iterator != _skills.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto DataReaderManager::LoadMotionEffect() -> void
{
	xml_document doc;
	for (const auto& [skillId, skill] : _skills)
	{
		for (const auto& motion : skill->motions)
		{
			//../../Binary/Resources/Xml/NPC/Skill/TurtleBabyBroccoliGreen/Eff_TurtleBabyBroccoliGreen_Attack_01_A.xml
			std::wstring motionPath(L"../../Binary/Resources/Xml/");
			motionPath.append(motion->motion_effect);
			const auto err = doc.load_file(StringUtils::ConvertWtoC(motionPath.c_str()).c_str());
			if (err.status == status_ok)
			{
				const auto effectNodes = doc.select_nodes("ms2/effect");

				for (const auto& effectNode : effectNodes)
				{
					auto effectObject = CreateEffectNode(effectNode);
					_motion_effects.emplace(motion, effectObject);
				}
			}
		}
	}
}

auto DataReaderManager::LoadTagEffect() -> void
{
	xml_document doc;
	for (const auto& [skillId, skill] : _skills)
	{
		for (const auto& motion : skill->motions)
		{
			//../../Binary/Resources/Xml/NPC/Skill/TurtleBabyBroccoliGreen/Eff_TurtleBabyBroccoliGreen_Attack_01_A.xml
			auto motionPaths = StringUtils::Split(motion->str_tag_effects, L',');
			std::vector<std::shared_ptr<Effect>> effects;
			for (const auto& motionPath : motionPaths)
			{
				auto realMotion = StringUtils::Split(motionPath, L'=');
				if (!realMotion.empty())
				{
					std::wstring motionPath(L"../../Binary/Resources/Xml/");
					motionPath.append(realMotion[1]);
					const auto err = doc.load_file(StringUtils::ConvertWtoC(motionPath.c_str()).c_str());
					if (err.status == status_ok)
					{
						const auto effectNodes = doc.select_nodes("ms2/effect");

						for (auto effectNode : effectNodes)
						{
							auto effectObject = CreateEffectNode(effectNode);
							effects.push_back(effectObject);
						}
					}
				}
			}
			if (!effects.empty())
			{
				_str_tag_effects.emplace(motion, effects);
			}
		}
	}
}

auto DataReaderManager::CreateEffectNode(const pugi::xpath_node effectNode) -> std::shared_ptr<Effect>
{
	auto effectObject = std::make_shared<Effect>();
	auto effect = effectNode.node();
	const std::string typeName(effect.attribute("type").value());
	if (typeName == "nif")
	{
		effectObject->type = kEffectType::kNif;
	}
	else if (typeName == "soundEvent")
	{
		effectObject->type = kEffectType::kSound;
	}
	else
	{
		effectObject->type = kEffectType::kEnd;
	}

	effectObject->file_path = effect.attribute("filePath").empty() ? L"" : StringUtils::ConvertCtoW(effect.attribute("filePath").value());
	effectObject->event = effect.attribute("event").empty() ? L"" : StringUtils::ConvertCtoW(effect.attribute("event").value());
	effectObject->group = effect.attribute("group").empty() ? L"" : StringUtils::ConvertCtoW(effect.attribute("group").value());
	effectObject->start_delay = effect.attribute("startDelay").empty() ? 0.f : std::stof(effect.attribute("startDelay").value());
	effectObject->duration = effect.attribute("duration").empty() ? 0.f : std::stof(effect.attribute("duration").value());
	return effectObject;
}

auto DataReaderManager::FindMotionEffect(const std::shared_ptr<Motion> key) -> std::shared_ptr<Effect>
{
	if (const auto& iterator = _motion_effects.find(key); iterator != _motion_effects.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto DataReaderManager::FindStrTagEffect(const std::shared_ptr<Motion> key) -> std::vector<std::shared_ptr<Effect>>
{
	if (const auto& iterator = _str_tag_effects.find(key); iterator != _str_tag_effects.end())
	{
		return iterator->second;
	}
	return std::vector<std::shared_ptr<Effect>>();
}

#pragma endregion
