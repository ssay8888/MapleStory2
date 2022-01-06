#include "game_server_pch.h"
#include "game_monster.h"

#include "data_reader/data_reader_manager.h"
#include "spawn_point/spawn_point.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"

static std::atomic<int64_t> npc_object_id = 1;

GameMonster::GameMonster(const std::shared_ptr<SpawnPoint>& spawnPoint) :
	_object_id(npc_object_id++),
	_spawn_point(spawnPoint)
{
}

auto GameMonster::Create(const std::shared_ptr<SpawnPoint>& spawnPoint) -> std::shared_ptr<GameMonster>
{
	auto instance = MakeShared<GameMonster>(spawnPoint);

	if (FAILED(instance->NativeContruct()))
	{
		return nullptr;
	}
	return instance;
}

auto GameMonster::GetObjectId() const -> int64_t
{
	return _object_id;
}

auto GameMonster::GetTransform() const -> std::shared_ptr<Transform>
{
	return _transform;
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
		monsterInfo->collision.width / 2.f / 100.f,
		monsterInfo->collision.height / 2.f / 100.f,
		monsterInfo->collision.depth / 2.f / 100.f);
	ColliderDesc.init_pos = _float3(0.f, (monsterInfo->collision.height / 2 / 100.f) * 0.5f, 0.f);

	const auto& componentManager = ComponentManager::GetInstance();
	auto component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
	_aabb_com = std::static_pointer_cast<Collider>(component);
	_aabb_com->NativeConstruct(&ColliderDesc);

	return S_OK;
}
