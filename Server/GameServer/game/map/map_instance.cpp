#include "game_server_pch.h"
#include "map_instance.h"

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/inventorys/inventorys.h"
#include "game_session/game_client_packet_handler.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "src/utility/components/transform/transform.h"
#include "string_utils/string_utils.h"
#include "game/map/map_object/map_object.h"

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
			for (auto eqp : eqpItems)
			{
				auto item = respawnTargetPlayerPkt.add_items();
				item->set_position(eqp.first);
				item->set_itemid(eqp.second);
			}

			session->Send(GameClientPacketHandler::MakeSendBuffer(respawnTargetPlayerPkt));
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
	if (_spawn_points.size() >= index)
	{
		return nullptr;
	}
	return &_spawn_points[index];
}

auto MapInstance::SetRegionPoint(std::vector<_float3> sp) -> void
{
	_region_points = sp;
}

auto MapInstance::GetRegionPoint(const int32_t index) -> const _float3* const
{
	if (_region_points.size() >= index)
	{
		return nullptr;
	}
	return &_region_points[index];
}

auto MapInstance::AddObjects(std::vector<MapManager::MapEntity> objects) -> void
{
	for (auto& object : objects)
	{
		//auto block = MapObject::Create(object);
		//_objects.push_back(block);
	}
}

auto MapInstance::GetObjects() const -> std::vector<std::shared_ptr<MapObject>>
{
	return _objects;
}
