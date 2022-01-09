#include "c_pch.h"
#include "monster_state.h"

#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/monster/monster.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto MonsterState::GravityMonster(double timeDelta, std::shared_ptr<Monster> monster) -> bool
{

	constexpr double gravity = 3.0;
	auto transform = monster->GetTransform();
	auto pos = transform->GetState(Transform::kState::kStatePosition);
	if (_map_objects.empty())
	{
		auto pos = transform->GetState(Transform::kState::kStatePosition);
		pos.y -= static_cast<float>(gravity * timeDelta);
		transform->SetState(Transform::kState::kStatePosition, pos);
		return true;
	}
	bool check = false;
	for (const auto& object : _map_objects)
	{
		if (object->GetCollider()->CollisionAabb(monster->GetMonsterColliderAabb()))
		{
			auto targetPos = object->GetTransform()->GetState(Transform::kState::kStatePosition);
			if (targetPos.x - 0.29f <= pos.x && targetPos.x + 0.29f >= pos.x &&
				targetPos.z - 0.29f <= pos.z && targetPos.z + 0.29f >= pos.z)
			{
				_last_tile_map_object = object;
				check = true;
				break;
			}
		}
	}
	if (!check)
	{
		auto pos = transform->GetState(Transform::kState::kStatePosition);
		pos.y -= static_cast<float>(gravity * timeDelta);
		transform->SetState(Transform::kState::kStatePosition, pos);
	}
	else
	{
		if (_last_tile_map_object)
		{
			if (monster->GetMonsterColliderAabb()->GetMin().y >= _last_tile_map_object->GetCollider()->GetMax().y)
			{
				return false;
			}
			auto pos = transform->GetState(Transform::kState::kStatePosition);
			pos.y = _last_tile_map_object->GetCollider()->GetMax().y;
			transform->SetState(Transform::kState::kStatePosition, pos);
		}
	}
	return true;
}

auto MonsterState::BlockUpCheck(std::shared_ptr<Monster> monster) -> bool
{
	if (_last_tile_map_object != nullptr)
	{
		auto pos = monster->GetTransform()->GetState(Transform::kState::kStatePosition);
		auto transform = _last_tile_map_object->GetTransform();
		auto targetPos = transform->GetState(Transform::kState::kStatePosition);

		if (targetPos.x - 0.29f <= pos.x && targetPos.x + 0.29f >= pos.x &&
			targetPos.z - 0.29f <= pos.z && targetPos.z + 0.29f >= pos.z)
		{
			return true;
		}
	}
	return false;
}

auto MonsterState::StraightCheck(std::shared_ptr<Monster> monster) -> bool
{
	if (_map_objects.empty())
	{
		return false;
	}
	bool check = false;
	auto collider = monster->GetMonsterColliderAabb();
	for (const auto& object : _map_objects)
	{
		if (collider->GetMin().y >= object->GetCollider()->GetMax().y)
		{
			continue;
		}
		if (object->GetCollider()->CollisionAabb(collider))
		{
			check = true;
			_last_wall_map_object = object;
		}
	}
	if (check)
	{
		if (_last_wall_map_object)
		{
			auto targetMin = _last_wall_map_object->GetCollider()->GetMin();
			auto Min = collider->GetMin();
			auto targetMax = _last_wall_map_object->GetCollider()->GetMax();
			auto Max = collider->GetMax();

			auto maxValue = std::max(Min.z, targetMin.z);
			auto minValue = min(Max.z, targetMax.z);
			auto result = minValue - maxValue;
			auto pos = monster->GetTransform()->GetState(Transform::kState::kStatePosition);
			//pos.z = _last_tile_map_object->GetCollider()->GetMin().z;
			//	_transform_com->SetState(Transform::kState::kStatePosition, pos);
		}
	}
	return check;
}

auto MonsterState::LoadLastTile(std::shared_ptr<Monster> monster) -> std::shared_ptr<MapObject>
{
	auto transform = monster->GetTransform();
	auto pos = transform->GetState(Transform::kState::kStatePosition);
	for (const auto& object : _map_objects)
	{
		if (object->GetCollider()->CollisionAabb(monster->GetMonsterColliderAabb()))
		{
			auto targetPos = object->GetTransform()->GetState(Transform::kState::kStatePosition);
			if (targetPos.x - 0.29f <= pos.x && targetPos.x + 0.29f >= pos.x &&
				targetPos.z - 0.29f <= pos.z && targetPos.z + 0.29f >= pos.z)
			{
				return object;
			}
		}
	}
	return nullptr;
}

auto MonsterState::ReloadMapObject(std::shared_ptr<Monster> monster, bool check) -> void
{
	auto range = monster->GetReloadRangeAabb();
	auto blockRange = monster->GetBlockRangeAabb();
	for (const auto& reaload : range)
	{
		if (!reaload->CollisionAabb(blockRange) || check)
		{
			blockRange->UpdateCollider();
			auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad");
			if (mapInstance)
			{
				_map_objects = mapInstance->FindRangeCellObject(blockRange);
				std::cout << "������ : " << _map_objects.size() << std::endl;
			}
			break;
		}
	}
}