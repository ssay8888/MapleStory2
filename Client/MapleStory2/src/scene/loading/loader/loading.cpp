#include "c_pch.h"
#include "loading.h"
#include <thread>

#include "data_reader/data_reader_manager.h"
#include "files_manager/file_manager.h"
#include "files_manager/items/item_model.h"
#include "protocol/game_protocol.pb.h"
#include "src/common/xml/map_parser.h"
#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/game_object/coat/coat.h"
#include "src/game_object/item/Weapon.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/monster/monster.h"
#include "src/game_object/numbers/numbers.h"
#include "src/game_object/pants/pants.h"
#include "src/game_object/player/player.h"
#include "src/game_object/sky/sky.h"
#include "src/game_object/terrain/terrain.h"
#include "src/game_object/ui/character_select/character_select_ui.h"
#include "src/game_object/ui/character_select/character_beauty/character_beauty_ui.h"
#include "src/game_object/ui/equipped_ui/equipped_ui.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/ket_set_ui/key_set_manager.h"
#include "src/game_object/ui/player_info/player_info.h"
#include "src/game_object/ui/skill_ui/skill_ui.h"
#include "src/game_object/user/user.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/static/mesh_static.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_cube/vi_buffer_cube.h"
#include "src/utility/components/vi_buffer/vi_buffer_terrain/vi_buffer_terrain.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utils/file_utils.h"
#include "string_utils/string_utils.h"

Loading::Loading(const ComPtr<IDirect3DDevice9>& device): 
	_graphic_device(device)
{
}

auto Loading::NativeConstruct(const kScene scene)->HRESULT
{
	_next_level = scene;

	_system_message.append(L"로딩을 시작합니다.");
	auto a = std::thread([=]()
		{
			ThreadMain();
		});
	a.detach();

	return S_OK;
}

auto Loading::Create(const ComPtr<IDirect3DDevice9>& device, const kScene nextLevel) -> std::shared_ptr<Loading>
{
	auto pInstance = std::make_shared<Loading>(device);

	if (FAILED(pInstance->NativeConstruct(nextLevel)))
	{
		MSGBOX("Failed to Creating Loading");
		return nullptr;
	}


	return pInstance;
}

auto Loading::ThreadMain()->HRESULT
{
	WRITE_LOCK;
	HRESULT hr = 0;

	switch (this->_next_level)
	{
	case kSceneCharacterSelect:
		hr = this->ReadyCharacterSelect();
		break;
	case kSceneGameInit:
		hr = this->ReadyInGameLoading();
		break;
	case kSceneGamePlay0:
		hr = this->ReadyGamePlay0();
		break;
	case kSceneGamePlay1:
		hr = this->ReadyGamePlay1();
		break;
	default: 
		break;
	}

	if (FAILED(hr))
		return E_FAIL;
	

	return S_OK;
}

auto Loading::ReadyCharacterSelect() -> HRESULT
{
	_system_message.append(L"캐선창");
	auto& objectManager = ObjectManager::GetInstance();
	auto& componentManager = ComponentManager::GetInstance();

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Fittingdool"), Fittingdoll::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Weapon"), Weapon::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Coat"), Coat::Create(_graphic_device))))
		return E_FAIL;

	
	auto animationNames = DataReaderManager::GetInstance().AllAnimationName();

	for (const auto& animation : animationNames)
	{
		std::wstring prototypeName(TEXT("Prototype_Mesh_Ani_"));
		auto aniName = FileUtils::ConvertCtoW(animation->animation_name.c_str());
		prototypeName.append(aniName);
		aniName.append(TEXT(".x"));

		if (FAILED(componentManager.AddPrototype(kSceneStatic, prototypeName, MeshDynamic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/DynamicMesh/MaplePlayer/"), aniName))))
			return E_FAIL;
	}
	for (const auto& animation : animationNames)
	{
		std::wstring prototypeName(TEXT("Prototype_Mesh_Ani_F_"));
		auto aniName = FileUtils::ConvertCtoW(animation->animation_name.c_str());
		prototypeName.append(aniName);
		aniName.append(TEXT(".x"));

		if (FAILED(componentManager.AddPrototype(kSceneStatic, prototypeName, MeshDynamic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/DynamicMesh/MaplePlayerF/"), aniName))))
			return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kSceneCharacterSelect, TEXT("Prototype_Texture_Filter"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Terrain/Filter.bmp")))))
		return E_FAIL;						 
											 
	if (FAILED(componentManager.AddPrototype(kSceneStatic, TEXT("Prototype_Shader_Mesh"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/Shader_Mesh.hlsl")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_GameSky"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/bg_perion_ch.dds")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_GameSky_Henesys"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/bg_henesys_a.dds")))))
		return E_FAIL;

	const auto modelList = MapParser::MapModelNameListExport("character_perion");
	for (auto& model : modelList)
	{
		componentManager.AddPrototype(kSceneCharacterSelect,
			std::wstring(L"Prototype_Mesh_Cube_").append(FileUtils::ConvertCtoW(model.c_str())),
			MeshStatic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/StaticMesh/Cube/"),
				FileUtils::ConvertCtoW(model.c_str()).append(L".X")));
	}

	MapManager::GetInstance().LoadCharacterInstance(kSceneCharacterSelect);
	if (FAILED(LoadCharacterSelectUi()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadCharacterBeautyUi()))
	{
		return E_FAIL;
	}

	auto models = DataReaderManager::GetInstance().AllItemModel();

	for (auto& model : models)
	{
		for (auto slot : model.second->GetSlots().slot)
		{
			for (auto asset : slot.second->asset)
			{
				if (FileManager::IsFileAccess(asset->name))
				{
					std::wstring prototypeName(L"Prototype_ItemModel_");

					prototypeName.append(std::to_wstring(model.first)).append(L"_").append(std::to_wstring(asset->gender));
					std::wstring path = FileManager::GetPath(StringUtils::ConvertCtoW(asset->name.c_str())).append(L"/");
					auto fileName = StringUtils::ConvertCtoW(FileManager::GetFileName(asset->name).c_str());
					 
					if (model.first >= 15000000 && model.first <= 15009999)
					{
						if (SUCCEEDED(componentManager.AddPrototype(kSceneStatic,
							prototypeName, 
							MeshStatic::Create(_graphic_device, path, fileName, model.first))))
						{
							std::cout << "Load File Succeeded : " << asset->name << std::endl;
							std::cout << " ㄴ 아이템아이디 : " << model.first << std::endl;
							std::wcout << " ㄴ 프로토타입 : " << prototypeName << std::endl;

							std::wstring iconPrototypeName(TEXT("Prototype_Texture_"));
							iconPrototypeName.append(std::to_wstring(model.first));
							std::wstring iconPath(TEXT("../../Binary/Resources/Image/item/icon/"));
							iconPath.append(std::to_wstring(model.first)).append(L".png");

							if (FAILED(componentManager.AddPrototype(kSceneStatic, iconPrototypeName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, iconPath))))
							{
								std::cout << "icon 이미지를 불러오기에 실패했습니다. item code : " << model.first << std::endl;
							}
						}
					}
					else
					{
						if (SUCCEEDED(componentManager.AddPrototype(kScene::kSceneStatic,
							prototypeName,
							MeshDynamic::Create(_graphic_device, path, fileName, model.first))))
						{
							std::cout << "Load File Succeeded : " << asset->name << std::endl;
							std::cout << " ㄴ 아이템아이디 : " << model.first << std::endl;
							std::wcout << " ㄴ 프로토타입 : " << prototypeName << std::endl;

							std::wstring iconPrototypeName(TEXT("Prototype_Texture_"));
							iconPrototypeName.append(std::to_wstring(model.first));
							std::wstring iconPath(TEXT("../../Binary/Resources/Image/item/icon/"));
							iconPath.append(std::to_wstring(model.first)).append(L".png");

							if (FAILED(componentManager.AddPrototype(kSceneStatic, iconPrototypeName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, iconPath))))
							{
								std::cout << "icon 이미지를 불러오기에 실패했습니다. item code : " << model.first << std::endl;
							}
						}
					}
				}
				
			}
		}
	}
	_is_finish = true;
	return S_OK;
}

auto Loading::ReadyInGameLoading() -> HRESULT
{
	_is_finish = true;
	return S_OK;
}

auto Loading::LoadCharacterSelectUi() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	auto& componentManager = ComponentManager::GetInstance();

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Character_Select_Ui"), CharacterSelectUi::Create(_graphic_device))))
		return E_FAIL;
	//메인 프레임
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectFrame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/MainFrame.png")))))
		return E_FAIL;

	//캐릭터리스트 아이템
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Normal"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CharacterListNormal.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Over"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CharacterLisOver.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Select"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CharacterListSelect.png")))))
		return E_FAIL;


	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Start_Normal_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/StartBtNormal.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Start_Over_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/StartBtOver.png")))))
		return E_FAIL;

	std::wstring jobs[] {L"Night", L"Wizard", L"Ranger", L"Thief" , L"RuneBlader" , L"SoulBinder" ,
		L"Berserker" , L"Priest" , L"HeavyGunner" , L"Assassin" , L"Striker"};

	for (auto& job : jobs)
	{
		//일러스트 불러오기
		std::wstring prototypeName(L"Prototype_Texture_");
		prototypeName.append(job).append(L"_il");
		std::wstring pathName(L"../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/");
		pathName.append(job).append(L".png");
		if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, prototypeName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, pathName))))
			return E_FAIL;

		//캐릭터선택이미지 불러오기
		std::wstring prototypeSelName(L"Prototype_Texture_");
		prototypeSelName.append(L"Sel_").append(job).append(L"_il");
		std::wstring pathSelName(L"../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/");
		pathSelName.append(L"Sel_").append(job).append(L".png");
		if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, prototypeSelName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, pathSelName))))
			return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_BackGround"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/BackGround.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_SelectEffect"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/SelectEffect.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_Sel_FrontGround"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/Sel_FrontGround.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_Sel_ShowInfo"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/CreateUi/ShowInfo.png")))))
		return E_FAIL;
	
	return S_OK;
}

auto Loading::LoadCharacterBeautyUi() -> HRESULT
{
	auto& objectManager = ObjectManager::GetInstance();
	auto& componentManager = ComponentManager::GetInstance();



	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Character_Beauty_Ui"), CharacterBeautyUi::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyFrame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/BeautyFrame.png")))))
		return E_FAIL;

	//남자고르기
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexManBtn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/SexManButton_%d.png"), 3))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexManImage"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/SexMan_%d.png"), 2))))
		return E_FAIL;

	//여자고르기
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexGirlBtn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/SexGirlButton_%d.png"), 3))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexGirlImage"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/SexGirl_%d.png"), 2))))
		return E_FAIL;

	//꾸밀아이템선택창
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyItem"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/Item_%d.png"), 3))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyItemOver"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/ItemOver.png")))))
		return E_FAIL;

	//돌아가기버튼
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_ReturnBtn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/ReturnButton_%d.png"), 3))))
		return E_FAIL;

	//아이템리스트를 담아둘곳
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyList"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/ItemList.png")))))
		return E_FAIL;

	//if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Mesh_Coat"), MeshDynamic::Create(_graphic_device, TEXT("../../Binary/Resources/Model/Weapon/1/14/"), TEXT("11400001_f_viking.x")))))
	//	return E_FAIL;

	//네임생성버튼
	if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_CreateName"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/CharacterSelectUi/BeautyUi/NameCreate.png")))))
		return E_FAIL;
	//if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Mesh_Pants"), MeshDynamic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/DynamicMesh/Pants/"), TEXT("11500003_m_bronzewarrior.X")))))
	//	return E_FAIL;
	//if (FAILED(componentManager.AddPrototype(kScene::kSceneCharacterSelect, TEXT("Prototype_Mesh_Pants_Base"), MeshDynamic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/DynamicMesh/Pants/"), TEXT("11500001_m_basicsportwearpants.X")))))
	//	return E_FAIL;

	return S_OK;
}


auto Loading::ReadyGamePlay0()->HRESULT
{
	/* 원형 게임오브젝 객체를 생성ㅇ핞다. */
	_system_message.clear();
	_system_message.append(L"게임리소스를 생성합니다.");
	
	auto& objectManager = ObjectManager::GetInstance();
	auto& componentManager = ComponentManager::GetInstance();
	/* For.Prototype_Player*/
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Player"), Player::Create(_graphic_device))))
		return E_FAIL;


	/* For.Prototype_Terrain*/
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Terrain"), Terrain::Create(_graphic_device))))
		return E_FAIL;


	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Sky"), Sky::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Mesh_Monster"), Monster::Create(_graphic_device))))
		return E_FAIL;

	auto allMonster = DataReaderManager::GetInstance().AllMonsterInfo();
	for (auto monsterInfo : allMonster)
	{
		wchar_t path[MAX_PATH];
		swprintf_s(path, MAX_PATH, L"../../Binary/Resources/Meshes/DynamicMesh/NpcData/%d/", monsterInfo->id);
		auto filesPath = FileManager::GetDirFileName(path, L"*.X");

		for (auto filePath : filesPath)
		{
			auto fileName = FileManager::GetFileName(filePath);
			std::wstring prototypeName(TEXT("Prototype_Npc_"));
			prototypeName.append(std::to_wstring(monsterInfo->id)).append(L"_");
			StringUtils::ToLower(fileName);
			prototypeName.append(fileName);
			wchar_t path[MAX_PATH];
			swprintf_s(path, MAX_PATH, L"../../Binary/Resources/Meshes/DynamicMesh/NpcData/%d/", monsterInfo->id);
			if (FAILED(componentManager.AddPrototype(kSceneStatic, prototypeName,
				MeshDynamic::Create(_graphic_device, path, fileName))))
			{
				
			}
		}
	}


	/* For.Prototype_UI */
	//if (FAILED(objectManager->AddPrototype(TEXT("Prototype_UI"), UI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Effect */
	//if (FAILED(objectManager->AddPrototype(TEXT("Prototype_Effect"), Effect::Create(m_pGraphic_Device))))
	//	return E_FAIL;



	/* 원형 리소스 객체를 생성한다. */

	/* For.Prototype_Mesh_Stone*/
	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Mesh_Man"), MeshStatic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/StaticMesh/Player/"), TEXT("man.x")))))
		return E_FAIL;
	const auto modelList = MapParser::MapModelNameListExport("02000003_ad");
	for (auto& model : modelList)
	{
		componentManager.AddPrototype(kSceneGamePlay0,
		                              std::wstring(L"Prototype_Mesh_Cube_").append(FileUtils::ConvertCtoW(model.c_str())),
		                              MeshStatic::Create(_graphic_device, TEXT("../../Binary/Resources/Meshes/StaticMesh/Cube/"), 
		                                                 FileUtils::ConvertCtoW(model.c_str()).append(L".X")));
	}

	/* For.Prototype_Texture_Robby */
	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Robby"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Robby.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Sky"), Texture::Create(_graphic_device, Texture::kType::kTypeCube, TEXT("../../Binary/Resources/Textures/SkyBox/burger%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Texture_Terrain */
	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Terrain"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Terrain/Grass_%d.tga"), 2))))
		return E_FAIL;

	/* For.Prototype_VIBuffer_Terrain*/
	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Terrain"), ViBufferTerrain::Create(_graphic_device, TEXT("../../Binary/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Filter"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Terrain/Filter.bmp")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Cube"), ViBufferCube::Create(_graphic_device))))
		return E_FAIL;		   

	if (FAILED(componentManager.AddPrototype(static_cast<int32_t>(kScene::kSceneGamePlay0), TEXT("Prototype_Shader_Terrain"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/Shader_Terrain.hlsl")))))
		return E_FAIL;

	if (FAILED(LoadMainPlayerInfo()))
	{
		return E_FAIL;
	}

	if (FAILED(LoadInventory()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadItemIcon()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadItemInfoPopup()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadEquippedUi()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadFreeSet()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadSkillUi()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadHpUi()))
	{
		return E_FAIL;
	}
	if (FAILED(LoadNumberFont()))
	{
		return E_FAIL;
	}
	
	MapManager::GetInstance().LoadMapInstance(kSceneGamePlay0);
	

	_system_message.clear();
	_system_message.append(L"로딩이 완료되었습니다.");

	_is_finish = true;
	return S_OK;
}

auto Loading::LoadMainPlayerInfo() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();
	auto& objectManager = ObjectManager::GetInstance();
	/* For.Prototype_Player*/
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_PlayerInfo"), PlayerInfo::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kSceneStatic, TEXT("Prototype_Shader_Ui_Gage"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/Shader_ui_gage.hlsl")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_Frame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/frame_0%d.png"), 2))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_HpGage"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/hp_gage.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_MpGage"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/mp_gage.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_AttackKey"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/attack_key.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_JumpKey"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/jump_key.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_PickupKey"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/pickup_key.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_SkillSet"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/mainplayerinfo/skill_set.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_ExpBarFrame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/expbar/uiexpbar_frame.png")))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_ExpBarProgress"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/expbar/uiexpbar_progress.png")))))
		return E_FAIL;

	return S_OK;
}

auto Loading::LoadInventory() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();


	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Inventory"), Inventory::Create())))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Inventory_Frame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/inventory_ui/inventory/inventory_frame.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Inventory_Close_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/inventory_ui/inventory/inventory_close_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Inventory_Tab_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/inventory_ui/inventory/inventory_tab_%d.png"), 2))))
		return E_FAIL;


	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_EquippedUi"), EquippedUi::Create())))
		return E_FAIL;
	return S_OK;
}

auto Loading::LoadItemIcon() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();
	const auto itemInfo = DataReaderManager::GetInstance().AllItemInfo();
	for (const auto& [itemid, item] : itemInfo)
	{
		std::wstring iconPrototypeName = fmt::format(L"Prototype_Texture_{0}", itemid);
		std::wstring iconPath = fmt::format(L"../../Binary/Resources/Image/item/icon/{0}.png", item->property.slot_icon == L"" ? std::to_wstring(itemid) : item->property.slot_icon);
		if (FAILED(componentManager.AddPrototype(kSceneStatic, iconPrototypeName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, iconPath))))
		{
			std::cout << "icon 이미지를 불러오기에 실패했습니다. item code : " << itemid << std::endl;
		}
	}
	return S_OK;
}

auto Loading::LoadItemInfoPopup() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();


	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_ItemInfoPopup"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/iteminfo_popup/iteminfo_popup.png")))))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto Loading::LoadEquippedUi() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();


	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Equipped"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/equipped/equipped.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Statup_Btn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/equipped/statup_%d.png"), 4))))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto Loading::LoadFreeSet() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();

	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_KeySetManager"), KeySetManager::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Attack_Key"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/key_set/attack_key.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Jump_Key"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/key_set/jump_key.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PickUp_Key"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/key_set/pickup_key.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_KeySet"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/key_set/key_set.png")))))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto Loading::LoadSkillUi() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_SkillUi"), SkillUi::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_SkillUi"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/skillui/skillui.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_SkillUi_MinusBtn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/skillui/minus_%d.png"), 4))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_SkillUi_PlusBtn"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/skillui/plus_%d.png"), 4))))
	{
		return E_FAIL;
	}

	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_SkillUi_SkillSet"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/skillui/skillset.png")))))
	{
		return E_FAIL;
	}

	const auto files = FileManager::GetDirFileName(L"../../Binary/Resources/Image/skill/");

	for (const auto& filePath : files)
	{
		auto prototypeName = fmt::format(L"Prototype_Texture_Skill_Icon_{}", FileManager::GetFileName(filePath));

		if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, prototypeName, Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, filePath))))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

auto Loading::LoadHpUi() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();


	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Monster_Hp_Ui_Frame"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/monster_hp_ui/monster_hp_frame.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Monster_Hp_Ui_Bar"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/monster_hp_ui/monster_hpbar.png")))))
	{
		return E_FAIL;
	}
	return S_OK; 
}

auto Loading::LoadNumberFont() -> HRESULT
{
	const auto& componentManager = ComponentManager::GetInstance();
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Numbers"), Numbers::Create())))
	{
		return E_FAIL;
	}


	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Damage_Number"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/damage_font/damage_%d.png"), 10))))
	{
		return E_FAIL;
	}
	if (FAILED(componentManager.AddPrototype(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Player_Number"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/damage_font/player_%d.png"), 10))))
	{
		return E_FAIL;
	}
	return S_OK;
}

auto Loading::ReadyGamePlay1()->HRESULT
{
	_is_finish = true;
	return S_OK;
}

auto Loading::GetSystemMessage() const -> std::wstring
{
	return _system_message;
}

auto Loading::IsFinished() const -> bool
{
	return _is_finish;
}