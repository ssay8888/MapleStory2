#include "c_pch.h"
#include "monster.h"

#include "data_reader/data_reader_manager.h"
#include "randomizer/randomizer.h"
#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/player/player.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/meshes/dynamic/mesh_dynamic.h"
#include "src/utility/components/meshes/dynamic/animation/animation.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"
#include "states/attack_a_state/monster_attack_a_state.h"
#include "states/attack_b_state/monster_attack_b_state.h"
#include "states/bore_state/monster_bore_state.h"
#include "states/idle_state/monster_idle_state.h"
#include "states/run_state/monster_run_state.h"
#include "states/walk_state/monster_walk_state.h"
#include "stats/monster_stats.h"
#include "string_utils/string_utils.h"
#include <src/game_object/monster/states/attack_c_state/monster_attack_c_state.h>

Monster::Monster(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT Monster::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT Monster::NativeConstruct(void* arg)
{
	GameObject::NativeConstruct(arg);
	if (arg == nullptr)
	{
		return E_FAIL;
	}

	auto monsterInfo = static_cast<Protocol::GameServerRespawnMonster*>(arg);

	if (monsterInfo == nullptr)
	{
		return E_FAIL;
	}
	_monster_info = *monsterInfo;
	_monster_stat = MakeShared<MonsterStats>(_monster_info);

	if(FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	return S_OK;
}

int32_t Monster::Tick(const double timeDelta)
{
	_aabb_com->UpdateCollider();
	_current_monster_state->Tick(timeDelta, GetMonster());
	return GameObject::Tick(timeDelta);
}

int32_t Monster::LateTick(const double timeDelta)
{
	_current_monster_state->LateTick(timeDelta, GetMonster());
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderNonAlpha, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT Monster::Render()
{
	GameObject::Render();
	if (FAILED(SetUpConstantTable()))
		return E_FAIL;

	auto result = _shader_com->BeginShader(0);

	auto mesh = _mesh_list.begin()->second;

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

	_current_monster_state->Render(GetMonster());

	std::wstring str;

#ifdef _DEBUG
	_obb_com->RenderDebug();
	_aabb_com->RenderDebug();
	//_block_range_aabb_com->RenderDebug();
	//for (const auto& reaload : _reload_range_aabb_com)
	//{
	//	reaload->RenderDebug();
	//}
#endif
	return S_OK;
}

auto Monster::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = MakeShared<Monster>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto Monster::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<Monster>
{
	auto instance = MakeShared<Monster>(device);
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto Monster::GetTransform() const -> std::shared_ptr<Transform>
{
	return _transform_com;
}

auto Monster::GetMonsterColliderObb() const -> std::shared_ptr<Collider>
{
	return _obb_com;
}

auto Monster::GetMonsterColliderAabb() const -> std::shared_ptr<Collider>
{
	return _aabb_com;
}

auto Monster::GetReloadRangeAabb() const -> std::vector<std::shared_ptr<Collider>>
{
	return _reload_range_aabb_com;
}

auto Monster::GetBlockRangeAabb() const -> std::shared_ptr<Collider>
{
	return _block_range_aabb_com;
}

auto Monster::GetMonsterInfo() const -> Protocol::GameServerRespawnMonster
{
	return _monster_info;
}

auto Monster::GetAnimationPeriod(const Protocol::kMonsterState type) -> double
{
	LPD3DXANIMATIONSET		pAS = nullptr;
	_mesh_list.begin()->second->GetAnimation()->GetAnimationController()->GetAnimationSet(_state_index[type], &pAS);
	pAS->Release();
	return pAS->GetPeriod();
}

auto Monster::GetAnimationTimeAcc() -> double
{
	return _mesh_list.begin()->second->GetAnimation()->GetTimeAcc();
}

auto Monster::GetStateIndex(Protocol::kMonsterState state) -> int32_t
{
	return _state_index[state];
}

auto Monster::GetMonsterState(Protocol::kMonsterState state) -> std::shared_ptr<MonsterState>
{
	const auto iterator = _state_index.find(state);

	if (iterator != _state_index.end())
	{
		return _monster_states[iterator->second];
	}
	return nullptr;
}

auto Monster::GetCurrentState() const -> std::shared_ptr<MonsterState>
{
	return _current_monster_state;
}

auto Monster::PlayAnimation(const double timeDelta) -> void
{
	_mesh_list.begin()->second->PlayAnimation(timeDelta);
}

auto Monster::ChangeAnimation(const Protocol::kMonsterState type) -> void
{
	const auto iterator = _animaion_index.find(type);
	const auto iteratorState = _state_index.find(type);
	if (iterator != _animaion_index.end() && iteratorState != _state_index.end())
	{
		if (ChangeMonsterState(iteratorState->second))
		{
			auto result = _mesh_list.begin()->second->SetAnimationIndex(iterator->second);
		}
	}
}

auto Monster::ChangeMonsterState(const int32_t index) -> bool
{
	if (_current_monster_state != _monster_states[index])
	{
		_current_monster_state = _monster_states[index];
		_current_monster_state->Enter(GetMonster());
		return true;
	}
	return false;
}

auto Monster::ChangeSkillId(int32_t id) -> void
{
	_skill_id = id;
}

auto Monster::GetSkillId() const -> int32_t
{
	return _skill_id;
}

auto Monster::AddComponents() -> HRESULT
{

	const auto gameLogicManager = GameLogicQueue::GetInstance();
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(_monster_info.monster_id());
	/* Com_Transform */
	Transform::TransformDesc		transformDesc;
	transformDesc.speed_per_sec = monsterInfo->model.walk_speed / 150.f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);

	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Transform"), TEXT("Com_Transform"), reinterpret_cast<std::shared_ptr<Component>*>(&_transform_com), &transformDesc)))
		return E_FAIL;

	const auto right = _monster_info.right();
	const auto up = _monster_info.up();
	const auto look = _monster_info.look();
	const auto position = _monster_info.position();
	_transform_com->SetState(Transform::kState::kStateRight, _float3(right.x(), right.y(), right.z()));
	_transform_com->SetState(Transform::kState::kStateUp, _float3(up.x(), up.y(), up.z()));
	_transform_com->SetState(Transform::kState::kStateLook, _float3(look.x(), look.y(), look.z()));
	_transform_com->SetState(Transform::kState::kStatePosition, _float3(position.x(), position.y(), position.z()));


	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_Shader_Mesh"), TEXT("Com_Shader"), reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	Collider::TagColliderDesc		ColliderDesc;
	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(
		monsterInfo->collision.width ,
		monsterInfo->collision.height,
		monsterInfo->collision.depth);
	ColliderDesc.init_pos = _float3(0.f, (monsterInfo->collision.height) * 0.5f, 0.f);

	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_OBB"), TEXT("Com_Shose_Obb"), reinterpret_cast<std::shared_ptr<Component>*>(&_obb_com), &ColliderDesc)))
		return E_FAIL;
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Shose_AABB"), reinterpret_cast<std::shared_ptr<Component>*>(&_aabb_com), &ColliderDesc)))
		return E_FAIL;

	_aabb_com->GetTransform()->SetState(Transform::kState::kStateRight, _float3(right.x(), right.y(), right.z()));
	_aabb_com->GetTransform()->SetState(Transform::kState::kStateUp, _float3(up.x(), up.y(), up.z()));
	_aabb_com->GetTransform()->SetState(Transform::kState::kStateLook, _float3(look.x(), look.y(), look.z()));
	_aabb_com->GetTransform()->SetState(Transform::kState::kStatePosition, _float3(position.x(), position.y(), position.z()));


	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.init_pos = _float3(0.f, 0, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_NoTarget_AABB"), TEXT("Com_Block_AABB"), reinterpret_cast<std::shared_ptr<Component>*>(&_block_range_aabb_com), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.parent_matrix = &_transform_com->GetWorldMatrix();
	ColliderDesc.scale = _float3(0.1f, 0.1f, 0.1f);

	_reload_range_aabb_com.resize(Player::kReloadEnd);
	constexpr float initPos = 0.4f;
	ColliderDesc.init_pos = _float3(0.f, initPos + 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Up"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadUp]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, -initPos + 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Down"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadDown]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(-initPos, 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Left"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadLeft]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(initPos, 0.29f, 0.f);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Right"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadRight]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, 0.29f, initPos);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Front"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadFront]), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.init_pos = _float3(0.f, 0.29f, -initPos);
	if (FAILED(AddComponent(kSceneStatic, TEXT("Prototype_Collider_AABB"), TEXT("Com_Reload_AABB_Back"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_reload_range_aabb_com[Player::kReloadBack]), &ColliderDesc)))
		return E_FAIL;

	const int32_t idleIndex = AnimationLoad();
	if (FAILED(idleIndex))
	{
		return E_FAIL;
	}
	for (const auto& meshDynamic : _mesh_list) 
	{
		LPD3DXANIMATIONSET		pAS = nullptr;
		meshDynamic.second->GetAnimation()->GetAnimationController()->GetAnimationSet(0, &pAS);
		if (FAILED(_mesh_list.begin()->second->GetAnimation()->GetAnimationController()->RegisterAnimationSet(pAS)))
		{
			std::cout << "몬스터 애니메이션 추가도중 실패함" << std::endl;
		}
		pAS->Release();
	}
	ChangeAnimation(Protocol::kMonsterState::kIdleA);

	
	return S_OK;
}

auto Monster::AnimationLoad() -> int32_t
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(_monster_info.monster_id());
	const auto animationNames = DataReaderManager::GetInstance().FindAniKey(monsterInfo->id);

	if (animationNames == nullptr)
	{
		return E_FAIL;
	}
	int idleIndex = -1;
	std::shared_ptr<MeshDynamic> mesh;
	int index = 1;
	for (const auto animation : animationNames->seqs)
	{
		std::wstring prototypeName(TEXT("Prototype_Npc_"));
		auto Seq = animation.second;
		prototypeName.append(std::to_wstring(monsterInfo->id)).append(L"_");
		auto name = Seq->name;
		StringUtils::ToLower(name);
		prototypeName.append(name).append(L".x");
		std::wstring commponentTag(TEXT("Com_"));
		commponentTag.append(Seq->name);
		if (FAILED(AddComponent(kScene::kSceneStatic, prototypeName, commponentTag, reinterpret_cast<std::shared_ptr<Component>*>(&mesh))))
		{
			return E_FAIL;
		}
		if (Seq->name == L"Idle_A")
		{
			_current_monster_state = MakeShared<MonsterIdleState>();
			_current_monster_state->Enter(GetMonster());
			_monster_states.emplace(animation.first, _current_monster_state);
			_state_index.emplace(Protocol::kMonsterState::kIdleA, animation.first);
			_animaion_index.emplace(Protocol::kMonsterState::kIdleA, index++);
			idleIndex = animation.first;
			_mesh_list.emplace(animation.first, mesh);
		}
		else if (Seq->name.find(L"Walk_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterWalkState>());
			_state_index.emplace(Protocol::kMonsterState::kWalkA, animation.first);
			_animaion_index.emplace(Protocol::kMonsterState::kWalkA, index++);
			_mesh_list.emplace(animation.first, mesh);
		}
		else if (Seq->name.find(L"Bore_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterBoreState>());
			_state_index.emplace(Protocol::kMonsterState::kBoreA, animation.first);
			_animaion_index.emplace(Protocol::kMonsterState::kBoreA, index++);
			_mesh_list.emplace(animation.first, mesh);
		}
		else if (Seq->name.find(L"Run_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterRunState>());
			_state_index.emplace(Protocol::kMonsterState::kRunA, animation.first);
			_animaion_index.emplace(Protocol::kMonsterState::kRunA, index++);
			_mesh_list.emplace(animation.first, mesh);
		}
		else if (Seq->name.find(L"Attack_Idle_A") != std::wstring::npos)
		{
			
		}
		else if (Seq->name.find(L"Attack_") != std::wstring::npos)
		{
			auto splits = StringUtils::Split(Seq->name, L'_');
			if (splits.empty() || splits.size() < 2)
			{
				continue;
			}

			if (splits[2] == L"A")
			{
				auto iterator = _state_index.end();
				Protocol::kMonsterState type = Protocol::kMonsterState::kAttack1A;
				if (splits[1] == L"01")
				{
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"02")
				{
					type = Protocol::kMonsterState::kAttack2A;
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"03")
				{
					type = Protocol::kMonsterState::kAttack3A;
					iterator = _state_index.find(type);
				}
				else
				{
					int a = 0;
				}

				if (iterator == _state_index.end())
				{
					_monster_states.emplace(animation.first, MakeShared<MonsterAttackAState>(type));
					_state_index.emplace(type, animation.first);
					_animaion_index.emplace(type, index++);
					_mesh_list.emplace(animation.first, mesh);
				}
			}
			else if (splits[2] == L"B")
			{
				auto iterator = _state_index.end();
				Protocol::kMonsterState type = Protocol::kMonsterState::kAttack1B;
				if (splits[1] == L"01")
				{
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"02")
				{
					type = Protocol::kMonsterState::kAttack2B;
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"03")
				{
					type = Protocol::kMonsterState::kAttack3B;
					iterator = _state_index.find(type);
				}


				if (iterator == _state_index.end())
				{
					_monster_states.emplace(animation.first, MakeShared<MonsterAttackAState>(type));
					_state_index.emplace(type, animation.first);
					_animaion_index.emplace(type, index++);
					_mesh_list.emplace(animation.first, mesh);
				}
			}
			else if (splits[2] == L"C")
			{

				auto iterator = _state_index.end();
				Protocol::kMonsterState type = Protocol::kMonsterState::kAttack1C;
				if (splits[1] == L"01")
				{
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"02")
				{
					type = Protocol::kMonsterState::kAttack2C;
					iterator = _state_index.find(type);
				}
				else if (splits[1] == L"03")
				{
					type = Protocol::kMonsterState::kAttack3C;
					iterator = _state_index.find(type);
				}


				if (iterator == _state_index.end())
				{
					_monster_states.emplace(animation.first, MakeShared<MonsterAttackAState>(type));
					_state_index.emplace(type, animation.first);
					_animaion_index.emplace(type, index++);
					_mesh_list.emplace(animation.first, mesh);
				}
			}

		}
	}
	return idleIndex;
}

auto Monster::SetUpConstantTable() const -> HRESULT
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

auto Monster::GetMonster() -> std::shared_ptr<Monster>
{
	return std::static_pointer_cast<Monster>(shared_from_this());
}
