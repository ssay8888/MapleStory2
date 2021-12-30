#include "game_server_pch.h"
#include "map_instance.h"

#include "game/entitiy/character/game_character.h"
#include "game_session/game_client_packet_handler.h"
#include "src/utility/components/transform/transform.h"
#include "string_utils/string_utils.h"

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
	for (auto [key, targetSession] : _characters)
	{
		if (targetSession->GetAccountId() != session->GetAccountId()) // 본인을 제외한 모든 맵안에있는 캐릭터들의 정보를 쏴준다.
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
			targetSession->Send(GameClientPacketHandler::MakeSendBuffer(respawnPlayerPkt));

			player = targetSession->GetPlayer();
			respawnPlayerPkt.set_character_id(player->GetCharacterId());
			respawnPlayerPkt.set_name(StringUtils::ConvertWtoC(player->GetName().c_str()).c_str());
			respawnPlayerPkt.set_gender(player->GetGender());
			respawnPlayerPkt.set_face_id(player->GetFaceId());
			pos = player->GetTransForm()->GetState(Transform::kState::kStatePosition);
			respawnPlayerPkt.set_pos_x(pos.x);
			respawnPlayerPkt.set_pos_y(pos.y);
			respawnPlayerPkt.set_pos_z(pos.z);
			session->Send(GameClientPacketHandler::MakeSendBuffer(respawnPlayerPkt));
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

auto MapInstance::GetSpawnPoint(int32_t index) -> _float3
{
	return _spawn_points[index];
}
