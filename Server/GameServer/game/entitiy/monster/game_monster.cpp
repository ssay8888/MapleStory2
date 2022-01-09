#include "game_server_pch.h"
#include "game_monster.h"

#include <utility>

#include "data_reader/data_reader_manager.h"
#include "spawn_point/spawn_point.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"
#include "stat/monster_stat.h"
#include "states/monster_bore_state/monster_bore_state.h"
#include "states/monster_idle_state/monster_idle_state.h"
#include "states/monster_run_state/monster_run_state.h"
#include "states/monster_walk_state/monster_walk_state.h"
#include "string_utils/string_utils.h"


GameMonster::GameMonster(std::shared_ptr<SpawnPoint> spawnPoint, std::shared_ptr<MapInstance> mapInstance) :
	_map_instance(std::move(mapInstance)),
	_spawn_point(std::move(spawnPoint))
{
}

auto GameMonster::Tick(const double timeDelta) -> void
{
	_current_monster_state->Tick(timeDelta, GetMonster());
}

auto GameMonster::LateTick(const double timeDelta) -> void
{
	_current_monster_state->LateTick(timeDelta, GetMonster());
}

auto GameMonster::Create(const std::shared_ptr<SpawnPoint>& spawnPoint, const std::shared_ptr<MapInstance>& mapInstance) -> std::shared_ptr<GameMonster>
{
	auto instance = MakeShared<GameMonster>(spawnPoint, mapInstance);

	if (FAILED(instance->NativeContruct()))
	{
		return nullptr;
	}
	return instance;
}

auto GameMonster::GetTransform() const -> std::shared_ptr<Transform>
{
	return _transform;
}

auto GameMonster::GetSpawnPoint() const -> std::shared_ptr<SpawnPoint>
{
	return _spawn_point;
}

auto GameMonster::GetMapInstance() const -> std::shared_ptr<MapInstance>
{
	return _map_instance;
}

auto GameMonster::GetMonster() -> std::shared_ptr<GameMonster>
{
	return std::static_pointer_cast<GameMonster>(shared_from_this());
}

auto GameMonster::GetStateIndex(Protocol::kMonsterState state) -> int32_t
{
	return _state_index[state];
}

auto GameMonster::GetStat() const -> std::shared_ptr<MonsterStat>
{
	return _monster_stat;
}

auto GameMonster::GetMonsterColliderAabb() const -> std::shared_ptr<Collider>
{
	return _aabb_com;
}

auto GameMonster::GetReloadRangeAabb() const -> std::vector<std::shared_ptr<Collider>>
{
	return _reload_range_aabb_com;
}

auto GameMonster::GetBlockRangeAabb() const -> std::shared_ptr<Collider>
{
	return _block_range_aabb_com;
}

auto GameMonster::ChangeState(Protocol::kMonsterState state) -> void
{
	auto index = GetStateIndex(state);
	auto iterator = _monster_states.find(index);
	if (iterator != _monster_states.end())
	{
		_current_monster_state = iterator->second;
		_current_monster_state->Enter(GetMonster());
	}
}

auto GameMonster::ChangeTargetCharacter(std::shared_ptr<GameCharacter> character) -> void
{
	_target_character = character;
}

auto GameMonster::GetTargetCharacter() const -> std::shared_ptr<GameCharacter>
{
	return _target_character;
}

auto GameMonster::NativeContruct() -> HRESULT
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(_spawn_point->GetSpawnNpcId());
	Transform::TransformDesc transformDesc;
	transformDesc.speed_per_sec = monsterInfo->model.walk_speed / 150.f;
	transformDesc.rotation_per_sec = D3DXToRadian(90.0);
	_transform = Transform::Create(nullptr);
	_transform->NativeConstruct(&transformDesc);
	_transform->SetScale(monsterInfo->model.scale, monsterInfo->model.scale, monsterInfo->model.scale);
	_transform->SetState(Transform::kState::kStatePosition, _spawn_point->GetPosition());


	Collider::TagColliderDesc		ColliderDesc;
	ColliderDesc.parent_matrix = &_transform->GetWorldMatrix();
	ColliderDesc.scale = _float3(
		monsterInfo->collision.width,
		monsterInfo->collision.height,
		monsterInfo->collision.depth);
	ColliderDesc.init_pos = _float3(0.f, monsterInfo->collision.height * 0.5f, 0.f);

	const auto& componentManager = ComponentManager::GetInstance();
	auto component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_aabb_com = std::static_pointer_cast<Collider>(component);
	_aabb_com->NativeConstruct(&ColliderDesc);
	_aabb_com->UpdateCollider();


	ColliderDesc.parent_matrix = &_transform->GetWorldMatrix();
	ColliderDesc.scale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.init_pos = _float3(0.f, 0, 0.f);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_block_range_aabb_com = std::static_pointer_cast<Collider>(component);
	_block_range_aabb_com->NativeConstruct(&ColliderDesc);
	_block_range_aabb_com->UpdateCollider();

	ColliderDesc.parent_matrix = &_transform->GetWorldMatrix();
	ColliderDesc.scale = _float3(0.1f, 0.1f, 0.1f);

	_reload_range_aabb_com.resize(GameMonster::kReloadEnd);
	constexpr float initPos = 0.4f;
	ColliderDesc.init_pos = _float3(0.f, initPos + 0.29f, 0.f);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadUp] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadUp]->NativeConstruct(&ColliderDesc);

	ColliderDesc.init_pos = _float3(0.f, -initPos + 0.29f, 0.f);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadDown] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadDown]->NativeConstruct(&ColliderDesc);

	ColliderDesc.init_pos = _float3(-initPos, 0.29f, 0.f);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadLeft] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadLeft]->NativeConstruct(&ColliderDesc);

	ColliderDesc.init_pos = _float3(initPos, 0.29f, 0.f);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadRight] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadRight]->NativeConstruct(&ColliderDesc);

	ColliderDesc.init_pos = _float3(0.f, 0.29f, initPos);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadFront] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadFront]->NativeConstruct(&ColliderDesc);

	ColliderDesc.init_pos = _float3(0.f, 0.29f, -initPos);
	component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_reload_range_aabb_com[GameMonster::kReloadBack] = std::static_pointer_cast<Collider>(component);
	_reload_range_aabb_com[GameMonster::kReloadBack]->NativeConstruct(&ColliderDesc);

	for (auto reload : _reload_range_aabb_com)
	{
		//reload->UpdateCollider();
	}

	_monster_stat = MonsterStat::Create(monsterInfo->id);

	if (FAILED(AnimationLoad()))
	{
		return E_FAIL;
	}

	return S_OK;
}

auto GameMonster::AnimationLoad() -> HRESULT
{
	auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(_spawn_point->GetSpawnNpcId());
	const auto animationNames = DataReaderManager::GetInstance().FindAnyKey(monsterInfo->id);

	if (animationNames == nullptr)
	{
		return E_FAIL;
	}
	for (const auto& animation : animationNames->seqs)
	{
		std::wstring prototypeName(TEXT("Prototype_Npc_"));
		auto Seq = animation.second;
		prototypeName.append(std::to_wstring(monsterInfo->id)).append(L"_");
		auto name = Seq->name;
		StringUtils::ToLower(name);
		prototypeName.append(name).append(L".x");
		std::wstring commponentTag(TEXT("Com_"));
		commponentTag.append(Seq->name);
		if (Seq->name == L"Idle_A")
		{
			_current_monster_state = MakeShared<MonsterIdleState>();
			_current_monster_state->Enter(GetMonster());
			_monster_states.emplace(animation.first, _current_monster_state);
			_state_index.emplace(Protocol::kMonsterState::kIdleA, animation.first);
		}
		else if (Seq->name.find(L"Walk_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterWalkState>());
			_state_index.emplace(Protocol::kMonsterState::kWalkA, animation.first);
		}
		else if (Seq->name.find(L"Bore_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterBoreState>());
			_state_index.emplace(Protocol::kMonsterState::kBoreA, animation.first);
		}
		else if (Seq->name.find(L"Run_A") != std::wstring::npos)
		{
			_monster_states.emplace(animation.first, MakeShared<MonsterRunState>());
			_state_index.emplace(Protocol::kMonsterState::kRunA, animation.first);
		}
	}
	return S_OK;
}
