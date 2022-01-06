#include "game_server_pch.h"
#include "map_instance.h"

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/inventorys/inventorys.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game_session/game_client_packet_handler.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "src/utility/components/transform/transform.h"
#include "string_utils/string_utils.h"
#include "game/map/map_object/map_object.h"
#include "map_object/xblock/map_xblock.h"

MapInstance::MapInstance(const int32_t mapId):
	_map_id(mapId)
{
}

auto MapInstance::AddCharacter(std::shared_ptr<GameSession> session) -> bool
{
	const auto result = _characters.emplace(session->GetPlayer()->GetCharacterId(), session);
	return result.second;
}

auto MapInstance::RemoveCharacter(const int64_t characterId) -> bool
{
	const auto result = _characters.erase(characterId);
	return result;
}

auto MapInstance::FindCharacter(const int64_t characterId) -> std::shared_ptr<GameSession>
{
	const auto iterator = _characters.find(characterId);
	if (iterator != _characters.end())
	{
		return iterator->second;
	}
	return nullptr;
}

auto MapInstance::BroadCastAddCharacter(std::shared_ptr<GameSession> session) -> void
{
	Protocol::GameServerRespawnPlayer respawnPlayerPkt;
	auto player = session->GetPlayer();
	respawnPlayerPkt.set_character_id(player->GetCharacterId());
	respawnPlayerPkt.set_name(StringUtils::ConvertWtoC(player->GetName().c_str()).c_str());
	respawnPlayerPkt.set_gender(player->GetGender());
	respawnPlayerPkt.set_face_id(player->GetFaceId());
	auto pos = player->GetTransForm()->GetState(Transform::kState::kStatePosition);
	respawnPlayerPkt.set_pos_x(pos.x);
	respawnPlayerPkt.set_pos_y(pos.y);
	respawnPlayerPkt.set_pos_z(pos.z);
	auto baseInfo =
		CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, player->GetCharacterId());
	auto inventoryInfo = std::static_pointer_cast<Inventorys>(baseInfo);

	auto eqpItems = inventoryInfo->AllItems(Protocol::kInventoryEquipped);
	for (auto eqp : eqpItems)
	{
		auto item = respawnPlayerPkt.add_items();
		item->set_position(eqp.first);
		item->set_itemid(eqp.second);
	}

	for (auto [key, targetSession] : _characters)
	{
		if (targetSession->GetAccountId() != session->GetAccountId()) // 본인을 제외한 모든 맵안에있는 캐릭터들의 정보를 쏴준다.
		{
			targetSession->Send(GameClientPacketHandler::MakeSendBuffer(respawnPlayerPkt));

			Protocol::GameServerRespawnPlayer respawnTargetPlayerPkt;
			player = targetSession->GetPlayer();
			respawnTargetPlayerPkt.set_character_id(player->GetCharacterId());
			respawnTargetPlayerPkt.set_name(StringUtils::ConvertWtoC(player->GetName().c_str()).c_str());
			respawnTargetPlayerPkt.set_gender(player->GetGender());
			respawnTargetPlayerPkt.set_face_id(player->GetFaceId());
			pos = player->GetTransForm()->GetState(Transform::kState::kStatePosition);
			respawnTargetPlayerPkt.set_pos_x(pos.x);
			respawnTargetPlayerPkt.set_pos_y(pos.y);
			respawnTargetPlayerPkt.set_pos_z(pos.z);
			baseInfo =
				CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, player->GetCharacterId());
			inventoryInfo = std::static_pointer_cast<Inventorys>(baseInfo);

			eqpItems = inventoryInfo->AllItems(Protocol::kInventoryEquipped);
			for (const auto eqp : eqpItems)
			{
				const auto item = respawnTargetPlayerPkt.add_items();
				item->set_position(eqp.first);
				item->set_itemid(eqp.second);
			}

			session->Send(GameClientPacketHandler::MakeSendBuffer(respawnTargetPlayerPkt));
		}
	}
}

auto MapInstance::Respawn() -> void
{
	if (_characters.empty())
	{
		return;
	}

	for (const auto& regionPoint : _region_points)
	{
		const auto iterator = _monsters.find(regionPoint);

		if (iterator == _monsters.end())
		{
			auto monster = GameMonster::Create(regionPoint);
			_monsters.emplace(regionPoint, monster);

			Protocol::GameServerRespawnMonster sendPkt;
			const auto transform = monster->GetTransform();
			sendPkt.set_object_id(monster->GetObjectId());
			sendPkt.set_monster_id(regionPoint->GetSpawnNpcId());
			sendPkt.set_hp(50);
			sendPkt.set_is_spawn(true);

			const auto right = transform->GetState(Transform::kState::kStateRight);
			const auto sendRight = sendPkt.mutable_right();
			sendRight->set_x(right.x);
			sendRight->set_y(right.y);
			sendRight->set_z(right.z);

			const auto up = transform->GetState(Transform::kState::kStateUp);
			const auto sendUp = sendPkt.mutable_up();
			sendUp->set_x(up.x);
			sendUp->set_y(up.y);
			sendUp->set_z(up.z);

			const auto look = transform->GetState(Transform::kState::kStateLook);
			const auto sendLook = sendPkt.mutable_look();
			sendLook->set_x(look.x);
			sendLook->set_y(look.y);
			sendLook->set_z(look.z);

			const auto position = transform->GetState(Transform::kState::kStatePosition);
			const auto sendPosition = sendPkt.mutable_position();
			sendPosition->set_x(position.x);
			sendPosition->set_y(position.y);
			sendPosition->set_z(position.z);

			const auto sendBuf = GameClientPacketHandler::MakeSendBuffer(sendPkt);
			for (const auto& character : _characters)
			{
				character.second->Send(sendBuf);
			}
		}
	}
}

auto MapInstance::GetMapId() const -> int32_t
{
	return _map_id;
}

auto MapInstance::SetSpawnPoint(const std::vector<_float3> sp) -> void
{
	_spawn_points = sp;
}

auto MapInstance::GetSpawnPoint(const int32_t index) -> const _float3* const
{
	if (index >= _spawn_points.size())
	{
		return nullptr;
	}
	return &_spawn_points[index];
}

auto MapInstance::SetRegionPoint(std::vector<std::shared_ptr<SpawnPoint>> sp) -> void
{
	_region_points = sp;
}

auto MapInstance::GetRegionPoint(const int32_t index) -> std::shared_ptr<SpawnPoint>
{
	if (index >=_region_points.size())
	{
		return nullptr;
	}
	return _region_points[index];
}

auto MapInstance::AddObjects(std::vector<MapManager::MapEntity> objects) -> void
{
	for (auto& object : objects)
	{
		auto block = MapXblock::Create(object);
		_objects.push_back(block);
	}
}

auto MapInstance::GetObjects() const -> std::vector<std::shared_ptr<MapXblock>>
{
	return _objects;
}
