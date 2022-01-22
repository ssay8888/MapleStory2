#include "c_pch.h"
#include "player.h"

#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/equipped/equipped.h"
#include "src/game_object/item/Item.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/ui/equipped_ui/equipped_ui.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/inventory/inventory_tab_btn/inventory_tab_btn.h"
#include "src/game_object/ui/monster_hp_ui/monster_hp_ui.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/game_object/ui/skill_ui/skill_ui.h"
#include "src/managers/characters_manager/character.h"
#include "src/managers/character_stat/character_stat.h"
#include "src/managers/weapon_manager/weapon_manager.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/dynamic/animation/animation.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"
#include "states/idle_state/idle_state.h"
#include "string_utils/string_utils.h"

Player::Player(const ComPtr<IDirect3DDevice9>& device) :
	GameObject(device)
{
}

auto Player::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto Player::NativeConstruct(void* arg) -> HRESULT
{
	GameObject::NativeConstruct(arg);
	if (arg)
	{
		_info = *static_cast<PlayerInfo*>(arg);
	}
	if (FAILED(AddComponents()))
		return E_FAIL;
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	ChangeAnimation(kAnimationType::kIdle);
	ChangeCharacterState(IdleState::GetInstance());
	_last_sp_recovery = GetTickCount64();
	return S_OK;
}

auto Player::Tick(const double timeDelta) -> HRESULT
{
	_character_state->HandleInput();
	_character_state->Tick(timeDelta);

	OpenInventory();
	MapChange();
	if (!g_isChatActive && InputDevice::GetInstance().GetKeyPressing(DIK_SPACE))
	{
		auto pos = _transform_com->GetState(Transform::kState::kStatePosition);
		pos.y += 1.f;
		_transform_com->SetState(Transform::kState::kStatePosition, pos);
	}

	SpRecovery();
	return GameObject::Tick(timeDelta);
}

auto Player::LateTick(const double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	_character_state->LateTick(timeDelta);
	//PlayAnimation(timeDelta);
	//GravityPlayer(timeDelta);

	WeaponManager::GetInstance().LateTick();
	return GameObject::LateTick(timeDelta);
}

auto Player::Render() -> HRESULT
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);

	auto mesh = _character_mesh_list[0];

	size_t iNumMeshContainers = mesh->GetNumMeshContainer();

	for (uint32_t i = 0; i < iNumMeshContainers; ++i)
	{
		const uint32_t iNumMaterials = mesh->GetNumMaterials(i);

		mesh->UpdateSkinnedMesh(i);

		for (uint32_t j = 0; j < iNumMaterials; ++j)
		{
			if (FAILED(mesh->SetUpTextureOnShader(_shader_com, "g_DiffuseTexture", MeshMaterialTexture::kType::kTypeDiffuse, i, j)))
				return E_FAIL;

			_shader_com->Commit();

			mesh->Render(i, j);
		}
	}
	result = _shader_com->EndShader();

	_character_state->Render();
	WeaponManager::GetInstance().Render();
	int a = 0;

	std::wstring str;

	RECT rc = {
		0, 0,
		1000, 1000
	};
	str.append(L"라 : ").append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateRight).x)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateRight).y)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateRight).z)).append(L"\r\n");

	str.append(L"업 : ").append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateUp).x)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateUp).y)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateUp).z)).append(L"\r\n");

	str.append(L"룩 : ").append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateLook).x)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateLook).y)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStateLook).z)).append(L"\r\n");

	str.append(L"포 : ").append(std::to_wstring(_transform_com->GetState(Transform::kState::kStatePosition).x)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStatePosition).y)).append(L" / ")
		.append(std::to_wstring(_transform_com->GetState(Transform::kState::kStatePosition).z)).append(L"\r\n");

	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rc, 
		DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));

#ifdef _DEBUG
	_character_aabb_com->RenderDebug();
	_block_ragne_aabb_com->RenderDebug();
	for (const auto& reload : _reload_ragne_aabb_com)
	{
		reload->RenderDebug();
	}
#endif
	return S_OK;
}

auto Player::GetTransform() const -> std::shared_ptr<Transform>
{
	return _transform_com;
}

auto Player::GetCurrentDynamicMesh() -> std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>
{
	return std::make_pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>(static_cast<std::shared_ptr<MeshDynamic>>(_character_mesh_list[0]), static_cast<std::shared_ptr<MeshDynamic>>(_character_mesh_list[_current_mesh_num]));
}

auto Player::GetInfo() const -> PlayerInfo
{
	return _info;
}

auto Player::GetCharacterColliderAabb() const -> std::shared_ptr<Collider>
{
	return _character_aabb_com;
}

auto Player::GetReloadRangeAabb() const -> std::vector<std::shared_ptr<Collider>>
{
	return _reload_ragne_aabb_com;
}

auto Player::GetBlockRangeAabb() const -> std::shared_ptr<Collider>
{
	return _block_ragne_aabb_com;
}

auto Player::GetStartAttackTime() const -> uint64_t
{
	return _start_attack_time;
}

auto Player::IsStartAttackTime() const -> bool
{
	if (_start_attack_time + 5000 < GetTickCount64())
	{
		return true;
	}
	return false;
}

auto Player::SettingStartAttackTime() -> void
{
	_start_attack_time = GetTickCount64();
}

auto Player::ResetStartAttackTime() -> void
{
	_start_attack_time = 0;
}

auto Player::GetRadian() const -> float
{
	return _radian;
}

auto Player::SetRadian(const float radian) -> void
{
	_radian = radian;
}

auto Player::GetCurrentAnimationType() const -> kAnimationType
{
	return static_cast<kAnimationType>(_current_mesh_num);
}

auto Player::GetAnimationTimeAcc() -> double
{
	return _character_mesh_list[0]->GetAnimation()->GetTimeAcc();
}

auto Player::ChangeEqp(const GameContents::kEquipeType type, int32_t itemId)->void
{
	const auto& instance = ObjectManager::GetInstance();
	auto iter = _eqp_mesh.find(itemId);
	if (iter != _eqp_mesh.end())
	{
		switch (type)
		{
		case GameContents::kEquipeType::kPants:
			_character_mesh_list[0]->ChangeSkinnedMesh(iter->second, "PA_");
			_eqp_list->AddItem(type, itemId);
			break;
		case GameContents::kEquipeType::kCoat:
			_character_mesh_list[0]->ChangeSkinnedMesh(iter->second, "CL_");
			_eqp_list->AddItem(type, itemId);
			break;
		case GameContents::kEquipeType::kShoes:
			_character_mesh_list[0]->ChangeSkinnedMesh(iter->second, "SH_");
			_eqp_list->AddItem(type, itemId);
			break;
		default:
			return;
		}
	}
	else
	{

		std::wstring prototypeName(L"Prototype_ItemModel_");
		prototypeName.append(std::to_wstring(itemId)).append(L"_").append(std::to_wstring(_info.sex));
		std::wstring componentTag(L"Com");
		componentTag.append(std::to_wstring(itemId)).append(L"_").append(std::to_wstring(_info.sex));
		if (auto component = std::static_pointer_cast<MeshDynamic>(CloneComponent(kSceneStatic, prototypeName, componentTag, nullptr)))
		{
			if (type != GameContents::kEquipeType::kWeapon)
			{
				switch (type)
				{
				case GameContents::kEquipeType::kPants:
					_character_mesh_list[0]->ChangeSkinnedMesh(component, "PA_");
					_eqp_list->AddItem(type, itemId);
					break;
				case GameContents::kEquipeType::kCoat:
					_character_mesh_list[0]->ChangeSkinnedMesh(component, "CL_");
					_eqp_list->AddItem(type, itemId);
					break;
				case GameContents::kEquipeType::kShoes:
					_character_mesh_list[0]->ChangeSkinnedMesh(component, "SH_");
					_eqp_list->AddItem(type, itemId);
					break;
				case GameContents::kEquipeType::kFace:
				{
					auto texture = DataReaderManager::GetInstance().FindFace(itemId);
					_character_mesh_list[0]->ChangeFaceTexture(texture->diffuse_map[0]);
					break;
				}
				default:
					return;
				}
				_eqp_mesh.emplace(itemId, component);

				const auto playerMesh = this->GetCurrentDynamicMesh();
				const auto rootFrame = playerMesh.first->GetRootFrame();
				int32_t index = 0;
				component->TargetCombinedTransformationMatrices(component, playerMesh.first, component->GetRootFrame(), rootFrame, index);
			}
		}
		else
		{
			if (type != GameContents::kEquipeType::kWeapon)
			{
				switch (type)
				{
				case GameContents::kEquipeType::kPants:
					_character_mesh_list[0]->ChangeSkinnedMesh(nullptr, "PA_");
					_eqp_list->RemoveItem(type);
					break;
				case GameContents::kEquipeType::kCoat:
					_character_mesh_list[0]->ChangeSkinnedMesh(nullptr, "CL_");
					_eqp_list->RemoveItem(type);
					break;
				case GameContents::kEquipeType::kShoes:
					_character_mesh_list[0]->ChangeSkinnedMesh(nullptr, "SH_");
					_eqp_list->RemoveItem(type);
					break;
				case GameContents::kEquipeType::kFace:
				{
					auto texture = DataReaderManager::GetInstance().FindFace(itemId);
					_character_mesh_list[0]->ChangeFaceTexture(texture->diffuse_map[0]);
					break;
				}
				default:
					return;
				}

				//const auto playerMesh = this->GetCurrentDynamicMesh();
				//const auto rootFrame = playerMesh.first->GetRootFrame();
				//int32_t index = 0;
				//component->TargetCombinedTransformationMatrices(component, playerMesh.first, component->GetRootFrame(), rootFrame, index);
			}
		}

		switch (type)
		{
		case GameContents::kEquipeType::kFace:
		{
			auto texture = DataReaderManager::GetInstance().FindFace(itemId);
			_character_mesh_list[0]->ChangeFaceTexture(texture->diffuse_map[0]);
			_eqp_list->AddItem(type, itemId);
			break;
		}
		case GameContents::kEquipeType::kWeapon:
		{
			auto characterId = GameLogicQueue::GetInstance()->GetCharacterInfo().character_id();
			WeaponManager::GetInstance().RemoveWeapon(characterId);
			WeaponManager::GetInstance().AddWeapon(characterId, itemId, _transform_com);
			break;
		}
		default:
			return;
		}
	}
}

auto Player::GetEqpList() const -> std::shared_ptr<Equipped>
{
	return _eqp_list;
}

auto Player::ChangeCharacterState(const std::shared_ptr<CharacterState>& state)->void
{
	_character_state = state;
	_character_state->SetPlayer(std::static_pointer_cast<Player>(shared_from_this()));
	_character_state->Enter();
}

auto Player::GetCharacterState() const -> std::shared_ptr<CharacterState>
{
	return _character_state;
}

auto Player::ChangeAnimation(kAnimationType type) -> void
{
	_new_mesh_num = static_cast<int32_t>(type);
	_character_mesh_list[0]->SetAnimationIndex(_new_mesh_num);
	_current_mesh_num = _new_mesh_num;
}

auto Player::PlayAnimation(const double timeDelta) -> void
{
	auto mesh = _character_mesh_list[0];
	mesh->PlayAnimation(timeDelta);
}

auto Player::SetPlayAnimation(const double timeDelta) -> void
{
	auto mesh = _character_mesh_list[0];
	mesh->SetAnimationTime(timeDelta);
}

auto Player::AddComponents() -> HRESULT
{
	const auto gameLogicManager = GameLogicQueue::GetInstance();
	const auto characterInfo = gameLogicManager->GetCharacterInfo();
	CharacterStat::GetInstance();
	/* Com_Transform */
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 1.5f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	_info.sex = characterInfo.gender();
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(characterInfo.pos_x(), characterInfo.pos_y(), characterInfo.pos_z()));
	const auto animationNames = DataReaderManager::GetInstance().AllAnimationName();

	std::shared_ptr<MeshDynamic> mesh;
	for (auto animation : animationNames)
	{
		std::wstring prototypeName(TEXT("Prototype_Mesh_Ani_"));
		if (characterInfo.gender())
		{
			prototypeName.append(L"F_");
		}
		auto aniName = animation->animation_name;
		prototypeName.append(StringUtils::ConvertCtoW(aniName.c_str()));
		aniName.append(".x");
		if (FAILED(AddComponent(kScene::kSceneStatic, prototypeName, StringUtils::ConvertCtoW(aniName.c_str()), reinterpret_cast<std::shared_ptr<Component>*>(&mesh), &animation)))
			return E_FAIL;

		_character_mesh_list.push_back(mesh);
	}

	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	Collider::TagColliderDesc		ColliderDesc;
	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(0.25f, 0.50f, 0.29f);
	ColliderDesc.init_pos = _float3(0.f, 0.25f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Shose_AABB"), reinterpret_cast<std::shared_ptr<Component>*>(&_character_aabb_com), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.init_pos = _float3(0.f, 0, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_NoTarget_AABB"), TEXT("Com_Block_AABB"), reinterpret_cast<std::shared_ptr<Component>*>(&_block_ragne_aabb_com), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(0.1f, 0.1f, 0.1f);

	_reload_ragne_aabb_com.resize(kReloadEnd);
	constexpr float initPos = 0.4f;
	ColliderDesc.init_pos = _float3(0.f, initPos + 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Up"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadUp]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, -initPos + 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Down"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadDown]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(-initPos, 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Left"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadLeft]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(initPos, 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Right"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadRight]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, 0.29f, initPos);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Front"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadFront]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, 0.29f, -initPos);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Back"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_ragne_aabb_com[kReloadBack]), &ColliderDesc)))
		return E_FAIL;

	for (auto& meshDynamic : _character_mesh_list)
	{
		if (_character_mesh_list[0] != meshDynamic)
		{
			LPD3DXANIMATIONSET		pAS = nullptr;
			meshDynamic->GetAnimation()->GetAnimationController()->GetAnimationSet(0, &pAS);
			if (FAILED(_character_mesh_list[0]->GetAnimation()->GetAnimationController()->RegisterAnimationSet(pAS)))
			{
				std::cout << "애니메이션 추가도중 실패함" << std::endl;
			}
			pAS->Release();
		}
	}

	auto face = DataReaderManager::GetInstance().FindFace(characterInfo.face_id());
	if (face != 0)
	{
		_character_mesh_list[0]->ChangeFaceTexture(face->diffuse_map[0]);
	}
	else
	{
		_character_mesh_list[0]->ChangeFaceTexture(DataReaderManager::GetInstance().FindFace(300001)->diffuse_map[0]);
	}

	_eqp_list = Equipped::Create();
	for (auto item : characterInfo.items())
	{
		if (item.position() < 0)
		{
			ChangeEqp(GameContents::EquipeType(item.itemid()), item.itemid());
		}
	}
	auto& objectManager= ObjectManager::GetInstance();
	auto baseInventory = objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
	auto inventory = std::static_pointer_cast<Inventory>(baseInventory);
	if (inventory)
	{
		for (auto porotocolItem : characterInfo.items())
		{
			auto inventoryTab = inventory->GetInventoryTab(porotocolItem.inventory_type());
			Item::ItemInfo info;
			info.position = porotocolItem.position();
			info.item_id = porotocolItem.itemid();
			info.quantity = porotocolItem.quantity();
			info.str = porotocolItem.str();
			info.dex = porotocolItem.dex();
			info.int_ = porotocolItem.int_();
			info.luk = porotocolItem.luk();
			info.wap = porotocolItem.wap();
			info.type = porotocolItem.inventory_type();

			auto item = Item::Create(info);
			item->GetPopupInfo()->SetItemInfo(info);
			inventoryTab->AddItem(info.position, item);
		}
	}
	return S_OK;
}

auto Player::SetUpConstantTable() const -> HRESULT
{
	auto& pipeline = PipeLine::GetInstance();
	const auto view = pipeline.GetTransform(D3DTS_VIEW);
	const auto project = pipeline.GetTransform(D3DTS_PROJECTION);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", _transform_com->GetWorldMatrix(), sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &view, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &project, sizeof(_matrix));

	const D3DLIGHT9	lightDesc = LightManager::GetInstance().GetLightDesc(0);
	const auto lightDir = _float4(lightDesc.Direction, 0.f);
	result = _shader_com->SetUpConstantTable("g_vLightDir", &lightDir, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightDiffuse", &lightDesc.Diffuse, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightAmbient", &lightDesc.Ambient, sizeof(_float4));
	result = _shader_com->SetUpConstantTable("g_vLightSpecular", &lightDesc.Specular, sizeof(_float4));
	const auto camPos = _float4(pipeline.GetCamPosition(), 1.f);
	result = _shader_com->SetUpConstantTable("g_vCamPosition", &camPos, sizeof(_float4));
	return S_OK;
}

auto Player::OpenInventory() -> HRESULT
{
	if (!g_isWindowsActive || g_isChatActive)
	{
		return S_OK;
	}
	if (InputDevice::GetInstance().GetKeyDown(DIK_I))
	{
		const auto& instance = ObjectManager::GetInstance();

		if (const auto inventory = std::static_pointer_cast<Inventory>(instance.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0)))
		{
			inventory->ChangeShow();
		}
	}
	if (InputDevice::GetInstance().GetKeyDown(DIK_P))
	{
		const auto& instance = ObjectManager::GetInstance();

		if (const auto inventory = std::static_pointer_cast<EquippedUi>(instance.GetGameObjectPtr(kSceneGamePlay0, L"Layer_EquippedUi", 0)))
		{
			inventory->ChangeShow();
		}
	}
	if (InputDevice::GetInstance().GetKeyDown(DIK_K))
	{
		const auto& instance = ObjectManager::GetInstance();

		if (const auto inventory = std::static_pointer_cast<SkillUi>(instance.GetGameObjectPtr(kSceneGamePlay0, L"Layer_SkillUi", 0)))
		{
			inventory->ChangeShow();
		}
	}
	return S_OK;
}

auto Player::MapChange() -> HRESULT
{
	if (!g_isWindowsActive || g_isChatActive)
	{
		return S_OK;
	}

	if(InputDevice::GetInstance().GetKeyDown(DIK_SPACE))
	{
		if (auto instance= MapManager::GetInstance().FindMapInstance(CharacterStat::GetInstance().GetMapName()))
		{
			auto portalNumber = instance->CollisionPortal(_character_aabb_com);

			Protocol::GameClientChangeMap sendPkt;
			sendPkt.set_portal_id(portalNumber);
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));

		}
	}
	return S_OK;
}

auto Player::SpRecovery() -> void
{
	auto time = GetTickCount64();
	if (_last_sp_recovery + 1000 < time)
	{
		if (CharacterStat::GetInstance().GetMp() < CharacterStat::GetInstance().GetMaxMp())
		{
			Protocol::GameClientSpRecovery sendPkt;
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			_last_sp_recovery = time;
		}
	}
}

auto Player::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Player>
{
	auto instance = std::make_shared<Player>(device);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create Player");
		return nullptr;
	}
	return instance;
}

auto Player::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<Player>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Player");
		return nullptr;
	}
	return instance;
}
