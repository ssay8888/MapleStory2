#include "game_server_pch.h"
#include "game_server_player_handler.h"

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/stats/Statistic.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/stat/monster_stat.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "protocol/game_protocol.pb.h"

auto GameServerPlayerHandler::TakeDamage(const int64_t characterId, const int64_t monsterObjectId,
                                         GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr || gameSession->GetPlayer()->GetCharacterId() != characterId)
	{
		return;
	}
	auto player = gameSession->GetPlayer();

	const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId());

	if (mapInstance != nullptr)
	{
		const auto monster = mapInstance->FindMonster(monsterObjectId);
		if (monster)
		{
			const auto playerStat = std::static_pointer_cast<Statistic>(CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, characterId));
			if (playerStat)
			{
				playerStat->GainHp(-5);
				Protocol::GameServerTakeDamage sendPkt;
				sendPkt.set_character_id(characterId);
				sendPkt.set_damage(5);
				mapInstance->BroadCastMessage(sendPkt, nullptr);

				Protocol::GameServerUpdateStat sendStatPkt;
				sendStatPkt.set_type(Protocol::kHp);
				sendStatPkt.set_value(playerStat->GetHp());
				gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendStatPkt));
			}
		}
	}
}

auto GameServerPlayerHandler::AttackMonster(Protocol::GameClientAttackMonster pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}

	const auto player = gameSession->GetPlayer();

	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
	{
		for (const auto objectId : pkt.monster_obj_id())
		{
			if (const auto monster = mapInstance->FindMonster(objectId))
			{
				const auto stat = monster->GetStat();
				stat->GainHp(-5000);
				if (stat->IsDead())
				{
					mapInstance->RemoveMonster(objectId);
					Protocol::GameServerKillMonster sendPkt;
					sendPkt.set_monster_obj_id(objectId);
					mapInstance->BroadCastMessage(sendPkt, nullptr);
					auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
					auto statInfo = std::static_pointer_cast<Statistic>(baseInfo);

					if (statInfo)
					{
						statInfo->GainExp(5);
						Protocol::GameServerUpdateStat statPkt;
						statPkt.set_type(Protocol::kExp);
						statPkt.set_value(statInfo->GetExp());
						gameSession->Send(GameClientPacketHandler::MakeSendBuffer(statPkt));
					}
				}
				else
				{
					Protocol::GameServerMonsterStatUpdate sendPkt;
					sendPkt.set_monster_obj_id(objectId);
					sendPkt.set_type(Protocol::kMonsterHp);
					sendPkt.set_value(stat->GetHp());
					mapInstance->BroadCastMessage(sendPkt, nullptr);
				}
			}
		}
	}

}
