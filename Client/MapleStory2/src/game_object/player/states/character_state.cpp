#include "c_pch.h"
#include "character_state.h"

#include "protocol/game_protocol.pb.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/player/player.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

std::vector<std::shared_ptr<MapObject>> CharacterState::_map_objects;

auto CharacterState::GravityPlayer(const double timeDelta) -> bool
{
	constexpr double gravity = 3.0;
	auto transform = _player->GetTransform();
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
		if (object->GetCollider()->CollisionAabb(_player->GetCharacterColliderAabb()))
		{
			//object->GetCollider()->CollisionAabb(_character_aabb_com);
			_last_tile_map_object = object;
			check = true;
			break;
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
			if (_player->GetCharacterColliderAabb()->GetMin().y >= _last_tile_map_object->GetCollider()->GetMax().y)
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

auto CharacterState::GravityPlayerSendMessage(kAnimationType type) const ->void
{
	const auto transform = _player->GetTransform();
	Protocol::GameClientMovePlayer sendPkt;
	sendPkt.set_state(static_cast<int32_t>(type));
	sendPkt.set_radian(_player->GetRadian());
	const auto playerPos = transform->GetState(Transform::kState::kStatePosition);
	const auto position = sendPkt.mutable_position();
	position->set_x(playerPos.x);
	position->set_y(playerPos.y);
	position->set_z(playerPos.z);
	SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
}

auto CharacterState::StraightCheck() -> bool
{
	if (_map_objects.empty())
	{
		return false;
	}
	bool check = false;
	auto characterAabb = _player->GetCharacterColliderAabb();
	for (const auto& object : _map_objects)
	{
		if (characterAabb->GetMin().y >= object->GetCollider()->GetMax().y)
		{
			continue;
		}
		if (object->GetCollider()->CollisionAabb(characterAabb))
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
			auto Min = characterAabb->GetMin();
			auto targetMax = _last_wall_map_object->GetCollider()->GetMax();
			auto Max = characterAabb->GetMax();

			auto maxValue = std::max(Min.z, targetMin.z);
			auto minValue = min(Max.z, targetMax.z);
			auto result = minValue - maxValue;
			auto pos = _player->GetTransform()->GetState(Transform::kState::kStatePosition);
			//pos.z = _last_tile_map_object->GetCollider()->GetMin().z;
			//	_transform_com->SetState(Transform::kState::kStatePosition, pos);
		}
	}
	return check;
}

auto CharacterState::ReloadMapObject()->void
{
	auto range = _player->GetReloadRangeAabb();
	auto blockRange = _player->GetBlockRangeAabb();
	for (const auto& reaload : range)
	{
		if (!reaload->CollisionAabb(blockRange))
		{
			blockRange->UpdateCollider();
			auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad");
			_map_objects = mapInstance->FindRangeCellObject(blockRange);
			break;
		}
	}
}
