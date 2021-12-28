#include "c_pch.h"
#include "player.h"

#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/equipped/equipped.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/managers/characters_manager/character.h"
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

HRESULT Player::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Player::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (arg)
	{
		_info = *static_cast<PlayerInfo*>(arg);
	}
	if (FAILED(AddComponents()))
		return E_FAIL;
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(0.f, 0.f, 0.f));
	_transform_com->SetScale(0.01f, 0.01f, 0.01f);

	ChangeAnimation(kAnimationType::kIdle);
	ChangeCharacterState(IdleState::GetInstance());
	return S_OK;
}

int32_t Player::Tick(const double timeDelta)
{
	_character_state->HandleInput();
	_character_state->Tick(timeDelta);
	//if (InputDevice::GetInstance().GetKeyDown(DIK_X))
	//{
	//	_is_idle = false;
	//	_new_mesh_num = (_current_mesh_num + 1) % 4;
	//	_character_mesh_list[0]->SetAnimationIndex(_new_mesh_num);
	//	_current_mesh_num = _new_mesh_num;
	//}

	/*bool move = false;
	if (InputDevice::GetInstance().GetKeyPressing(DIK_UP) && InputDevice::GetInstance().GetKeyPressing(DIK_LEFT))
	{
		_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(135));
		move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_UP) && InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT))
	{
		_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(225));
		move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_DOWN) && InputDevice::GetInstance().GetKeyPressing(DIK_LEFT))
	{
		_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(45));
		move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_DOWN) && InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT))
	{
		_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(315));
		move = true;
	}
	else
	{
		if (InputDevice::GetInstance().GetKeyPressing(DIK_UP))
		{
			_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(180));
			move = true;
		}
		if (InputDevice::GetInstance().GetKeyPressing(DIK_LEFT))
		{
			_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(90));
			move = true;
		}
		if (InputDevice::GetInstance().GetKeyPressing(DIK_DOWN))
		{
			_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(0));
			move = true;
		}
		if (InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT))
		{
			_transform_com->SetUpRotation(_float3(0.f, 1.f, 0.f), D3DXToRadian(270));
			move = true;
		}
	}
	if (move)
	{
		_transform_com->BackStraight(timeDelta);
	}*/

	if (InputDevice::GetInstance().GetKeyPressing(DIK_SPACE))
	{
		auto pos = _transform_com->GetState(Transform::kState::kStatePosition);
		pos.y += 0.1f;
		_transform_com->SetState(Transform::kState::kStatePosition, pos);
	}
	//_character_aabb_com->UpdateCollider();
	//for (const auto& reload : _reload_ragne_aabb_com)
	//{
	//	reload->UpdateCollider();
	//}

	//if (move)
	//{
	//	if (StraightCheck())
	//	{
	//		_transform_com->BackStraight(-timeDelta);
	//	}
	//}
	//_character_aabb_com->UpdateCollider();
	//for (const auto& reload : _reload_ragne_aabb_com)
	//{
	//	reload->UpdateCollider();
	//}

	return GameObject::Tick(timeDelta);
}

int32_t Player::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	_character_state->LateTick(timeDelta);
	//PlayAnimation(timeDelta);
	//GravityPlayer(timeDelta);
	return GameObject::LateTick(timeDelta);
}

HRESULT Player::Render()
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

	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, str.c_str(), -1, &rc, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));

#ifdef _DEBUG
	_character_aabb_com->RenderDebug();
	_block_ragne_aabb_com->RenderDebug();
	for (const auto& reaload : _reload_ragne_aabb_com)
	{
		reaload->RenderDebug();
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

		switch (type)
		{
		case GameContents::kEquipeType::kFace:
		{
			auto texture = DataReaderManager::GetInstance().FindFace(itemId);
			_character_mesh_list[0]->ChangeFaceTexture(texture->diffuse_map[0]);
			_eqp_list->AddItem(type, itemId);
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

auto Player::AddComponents() -> HRESULT
{
	const auto gameLogicManager = GameLogicQueue::GetInstance();
	const auto characterInfo = gameLogicManager->GetCharacterInfo();
	/* Com_Transform */
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = 1.5f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	_info.sex = characterInfo.gender();
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;

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
	ColliderDesc.init_pos = _transform_com->GetState(Transform::kState::kStatePosition);
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
		ChangeEqp(GameContents::EquipeType(item.itemid()), item.itemid());
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
