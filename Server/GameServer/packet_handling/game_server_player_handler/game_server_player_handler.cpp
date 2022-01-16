#include "game_server_pch.h"
#include "game_server_player_handler.h"

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/inventorys/inventorys.h"
#include "game/entitiy/character/information_collection/stats/Statistic.h"
#include "game/entitiy/item/game_item.h"
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

auto GameServerPlayerHandler::InventoryItemMove(Protocol::GameClientInventoryItemMove pkt, GameSessionRef gameSession)->void
{
	if (gameSession->GetPlayer() == nullptr || pkt.src() >= kInventoryMaxSlot || pkt.dst() >= kInventoryMaxSlot)
	{
		return;
	}

	const auto player = gameSession->GetPlayer();
	const auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, player->GetCharacterId());
	const auto inventoryInfo = std::static_pointer_cast<Inventorys>(baseInfo);
	if (inventoryInfo)
	{
		switch (pkt.move_type())
		{
		case Protocol::kMove:
			inventoryInfo->SwapItem(pkt.type(), pkt.src(), pkt.dst());
			break;
		case Protocol::kEquipment:
		{
			const auto equippedInven = inventoryInfo->FindInventory(Protocol::kInventoryEquipped);
			const auto eqpInven = inventoryInfo->FindInventory(Protocol::kInventoryEqp);
			const auto oldEquippedItem = equippedInven->FindItem(pkt.dst());
			const auto oldEqpItem = eqpInven->FindItem(pkt.src());
			if (oldEqpItem == nullptr && eqpInven->FindItem(pkt.src()) != nullptr)
			{
				gameSession->Disconnect(L"인벤토리 패킷 조작");
				return;
			}
			auto dst = static_cast<int32_t>(GameContents::EquipeType(oldEqpItem->GetItemId()));
			equippedInven->RemoveItem(dst);
			equippedInven->PushItem(dst, oldEqpItem);
			oldEqpItem->SetPosition(dst);
			oldEqpItem->SetInventoryType(Protocol::kInventoryEquipped);
			eqpInven->RemoveItem(pkt.src());
			if (oldEquippedItem)
			{
				oldEquippedItem->SetPosition(pkt.src());
				oldEquippedItem->SetInventoryType(Protocol::kInventoryEqp);
				eqpInven->PushItem(pkt.src(), oldEquippedItem);
			}

			Protocol::GameServerDressChange sendPkt;
			sendPkt.set_character_id(player->GetCharacterId());
			sendPkt.set_item_type(dst);
			sendPkt.set_item_id(oldEqpItem->GetItemId());
			if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
			{
				mapInstance->BroadCastMessage(sendPkt, gameSession);
			}

			break;
		}
		case Protocol::kUnEquipment:
		{
			const auto equippedInven = inventoryInfo->FindInventory(Protocol::kInventoryEquipped);
			const auto eqpInven = inventoryInfo->FindInventory(Protocol::kInventoryEqp);
			const auto equippedItem = equippedInven->FindItem(pkt.src());

			if (equippedItem == nullptr || eqpInven->FindItem(pkt.dst()) != nullptr || pkt.src() >= 0)
			{
				gameSession->Disconnect(L"인벤토리 패킷 조작");
				return;
			}

			auto src = static_cast<int32_t>(GameContents::EquipeType(equippedItem->GetItemId()));
			equippedInven->RemoveItem(src);

			if (equippedItem)
			{
				equippedItem->SetPosition(pkt.dst());
				equippedItem->SetInventoryType(Protocol::kInventoryEqp);
				eqpInven->PushItem(pkt.dst(), equippedItem);
			}
			Protocol::GameServerDressChange sendPkt;
			sendPkt.set_character_id(player->GetCharacterId());
			sendPkt.set_item_type(src);
			sendPkt.set_item_id(0);
			if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
			{
				mapInstance->BroadCastMessage(sendPkt, gameSession);
			}

			break;
		}
		default:;
		}
	}
}
