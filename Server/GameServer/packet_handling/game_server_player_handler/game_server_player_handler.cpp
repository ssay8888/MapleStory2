#include "game_server_pch.h"
#include "game_server_player_handler.h"

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/inventorys/inventorys.h"
#include "game/entitiy/character/information_collection/keyset/keyset.h"
#include "game/entitiy/character/information_collection/stats/Statistic.h"
#include "game/entitiy/item/game_item.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/stat/monster_stat.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "protocol/game_protocol.pb.h"
#include "randomizer/randomizer.h"

#include <fmt/xchar.h>

#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/map/map_object/xblock/map_xblock.h"
#include "managers/game_job_queue/game_character_load_queue.h"
#include "src/utility/components/transform/transform.h"
#include "string_utils/string_utils.h"

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
				sendPkt.set_monster_obj_id(monsterObjectId);
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
		Protocol::GameServerAttackMonster attackMonster;
		attackMonster.set_character_id(player->GetCharacterId());
		for (const auto objectId : pkt.monster_obj_id())
		{
			if (const auto monster = mapInstance->FindMonster(objectId))
			{
				const auto stat = monster->GetStat();
				int64_t damage = Randomizer::Rand(10ll, 100ll);
				stat->GainHp(static_cast<int32_t>(-damage));
				auto damageItem = attackMonster.add_damages();
				damageItem->set_damage(damage);
				damageItem->set_monster_obj_id(monster->GetObjectId());

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
						auto monsterInfo = DataReaderManager::GetInstance().FindMonsterInfo(monster->GetSpawnPoint()->GetSpawnNpcId());
						statInfo->GainExp(30);
						Protocol::GameServerUpdateStat statPkt;
						statPkt.set_type(Protocol::kExp);
						statPkt.set_value(statInfo->GetExp());
						gameSession->Send(GameClientPacketHandler::MakeSendBuffer(statPkt));

						if (statInfo->GetExp() >= g_exp[statInfo->GetLevel()])
						{
							statInfo->LevelUp();
							Protocol::GameServerUpdateStat statPkt;
							statPkt.set_type(Protocol::kLevel);
							statPkt.set_value(statInfo->GetLevel());
							gameSession->Send(GameClientPacketHandler::MakeSendBuffer(statPkt));
						}
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
		mapInstance->BroadCastMessage(attackMonster, nullptr);
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

auto GameServerPlayerHandler::PlayerStatUp(Protocol::GameClientStatUp pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}

	const auto player = gameSession->GetPlayer();
	const auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
	const auto statInfo = std::static_pointer_cast<Statistic>(baseInfo);

	if (statInfo)
	{
		if (statInfo->GetAp() <= 0)
		{
			return;
		}

		Protocol::GameServerStatUp sendPkt;
		sendPkt.set_type(pkt.type());
		switch (pkt.type())
		{
		case Protocol::kStr:
			statInfo->SetStr(statInfo->GetStr() + 1);
			sendPkt.set_value(statInfo->GetStr());
			break;
		case Protocol::kDex:
			statInfo->SetDex(statInfo->GetDex() + 1);
			sendPkt.set_value(statInfo->GetDex());
			break;
		case Protocol::kInt:
			statInfo->SetInt(statInfo->GetInt() + 1);
			sendPkt.set_value(statInfo->GetInt());
			break;
		case Protocol::kLuk:
			statInfo->SetLuk(statInfo->GetLuk() + 1);
			sendPkt.set_value(statInfo->GetLuk());
			break;
		default: 
			return;
		}
		statInfo->SetAp(statInfo->GetAp() - 1);
		sendPkt.set_ap_value(statInfo->GetAp());

		gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt)); 
	}

}

auto GameServerPlayerHandler::PlayerKeySetChange(Protocol::GameClientKeySet pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr || pkt.key_value() < 0 || pkt.key_value() > 16)
	{
		return;
	}

	const auto player = gameSession->GetPlayer();
	const auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kKeyset, player->GetCharacterId());
	const auto keysetInfo = std::static_pointer_cast<Keyset>(baseInfo);
	if (keysetInfo)
	{
		if (pkt.value() == -1)
		{
			keysetInfo->RemoveItem(pkt.key_value());
		}
		else
		{
			auto index = keysetInfo->FindItem(pkt.key_value());
			if (index == -1)
			{
				keysetInfo->PushItem(pkt.key_value(), pkt.value());
			}
			else
			{
				keysetInfo->ChangeItem(pkt.key_value(), pkt.value());
			}
		}
	}
}

auto GameServerPlayerHandler::PlayerItemApply(Protocol::GameClientItemApply pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr || pkt.position() < 0 || pkt.position() > kInventoryMaxSlot)
	{
		return;
	}

	const auto& infoManager = CharacterInfoStorageManager::GetInstance();
	const auto player = gameSession->GetPlayer();
	const auto baseInfo = infoManager.FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, player->GetCharacterId());
	const auto inventoryInfo = std::static_pointer_cast<Inventorys>(baseInfo);

	if (inventoryInfo)
	{
		auto item = inventoryInfo->FindItem(pkt.type(), pkt.position());
		if (item == nullptr || item->GetQuantity() <= 0)
		{
			return;
		}
		item->SetQuantity(item->GetQuantity() - 1);
		const auto baseStats = infoManager.FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
		const auto statInfo = std::static_pointer_cast<Statistic>(baseStats);
		statInfo->SetHp(statInfo->GetHp() + 50);

		Protocol::GameServerUpdateStat sendStatPkt;
		sendStatPkt.set_type(Protocol::kHp);
		sendStatPkt.set_value(statInfo->GetHp());
		gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendStatPkt));

		if (item->GetQuantity() <= 0)
		{
			inventoryInfo->RemoveItem(pkt.type(), pkt.position());
		}
		Protocol::GameServerItemQuantityUpdate sendPkt;
		sendPkt.set_type(pkt.type());
		sendPkt.set_position(pkt.position());
		sendPkt.set_quantity(item->GetQuantity());
		gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt));
	}
}

auto GameServerPlayerHandler::PlayerResurrection(Protocol::GameClientResurrection pkt,
	GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}
	auto player = gameSession->GetPlayer();
	Protocol::GameServerResurrection sendPkt;
	sendPkt.set_character_id(player->GetCharacterId());
	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
	{
		auto spawnPoint = mapInstance->GetSpawnPoint(0);

		auto position = 	sendPkt.mutable_position();
		position->set_x(spawnPoint->x);
		position->set_y(spawnPoint->y);
		position->set_z(spawnPoint->z);
		mapInstance->BroadCastMessage(sendPkt, nullptr);
	}

	auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
	auto statInfo = std::static_pointer_cast<Statistic>(baseInfo);

	if (statInfo)
	{
		statInfo->SetHp(statInfo->GetMaxHp() / 2);

		Protocol::GameServerUpdateStat sendStatPkt;
		sendStatPkt.set_type(Protocol::kHp);
		sendStatPkt.set_value(statInfo->GetHp());
		gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendStatPkt));
	}
}

auto GameServerPlayerHandler::PlayerSpRecovry(Protocol::GameClientSpRecovery pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}
	
	auto player = gameSession->GetPlayer();

	if (player->IsLastSpRecovery())
	{
		player->SetLastSpRecovery(GetTickCount64());

		auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
		auto statInfo = std::static_pointer_cast<Statistic>(baseInfo);

		if (statInfo)
		{
			statInfo->SetMp(statInfo->GetMp() + 10);

			Protocol::GameServerUpdateStat sendStatPkt;
			sendStatPkt.set_type(Protocol::kMp);
			sendStatPkt.set_value(statInfo->GetMp());
			gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendStatPkt));
		}
	}

}

auto GameServerPlayerHandler::ApplySkill(Protocol::GameClientApplySkill pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}

	auto player = gameSession->GetPlayer();

	auto baseInfo = CharacterInfoStorageManager::GetInstance().FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
	auto statInfo = std::static_pointer_cast<Statistic>(baseInfo);

	if (statInfo)
	{
		constexpr int skills[] = { 10200001, 10200011, 10200031, 10200041 };
		switch (pkt.skillid())
		{
		case 10200011:
			statInfo->SetMp(statInfo->GetMp() - 4);
			break;
		case 10200041:
			statInfo->SetMp(statInfo->GetMp() - 24);
			break;
		}
		Protocol::GameServerUpdateStat sendStatPkt;
		sendStatPkt.set_type(Protocol::kMp);
		sendStatPkt.set_value(statInfo->GetMp());
		gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendStatPkt));
	}

}

auto GameServerPlayerHandler::GameChat(Protocol::GameClientChat pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr || pkt.contents().empty())
	{
		return;
	}

	auto player = gameSession->GetPlayer();
	

	Protocol::GameServerChat sendPkt;
	sendPkt.set_contents(fmt::format("{0} : {1}", StringUtils::ConvertWtoC(player->GetName().c_str()), pkt.contents()));


	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
	{
		mapInstance->BroadCastMessage(sendPkt, nullptr);
	}
}

auto GameServerPlayerHandler::ChangeMap(Protocol::GameClientChangeMap pkt, GameSessionRef gameSession) -> void
{
	if (gameSession->GetPlayer() == nullptr)
	{
		return;
	}

	auto player = gameSession->GetPlayer();
	auto transform = gameSession->GetPlayer()->GetTransform();

	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(player->GetMapId()))
	{
		if (mapInstance->PortalHackCheck(pkt.portal_id(), player->GetTransform()))
		{
			auto portal = 	mapInstance->FindPortal(pkt.portal_id());
			if (portal)
			{
				const auto changeMapInstance = MapManager::GetInstance().FindMapInstance(portal->GetChangeMapId());
				const auto changeMapPortal = changeMapInstance->FindPortal(0);
				player->SetMapId(portal->GetChangeMapId());
				mapInstance->RemoveCharacter(player->GetCharacterId());
				//changeMapInstance->AddCharacter(gameSession);
				//changeMapInstance->BroadCastAddCharacter(gameSession);
				auto pos = changeMapPortal->GetTransform()->GetState(Transform::kState::kStatePosition);
				transform->SetState(Transform::kState::kStatePosition, pos);

				Protocol::GameServerLoadCharacter sendPkt;
				GameCharacterLoadQueue::SettingCharacterInfoSendPacket(sendPkt, gameSession, gameSession->GetPlayer(), false);
				sendPkt.set_pos_x(pos.x);
				sendPkt.set_pos_y(pos.y);
				sendPkt.set_pos_z(pos.z);
				gameSession->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt));

				//Protocol::GameServerResurrection changePosition;
				//changePosition.set_character_id(player->GetCharacterId());
				//auto position = changePosition.mutable_position();
				//auto pos = transform->GetState(Transform::kState::kStatePosition);
				//position->set_x(pos.x);
				//position->set_y(pos.y);
				//position->set_z(pos.z);
				//gameSession->Send(GameClientPacketHandler::MakeSendBuffer(changePosition));

				Protocol::GameServerChangeMap changePkt;
				changePkt.set_map_id(portal->GetChangeMapId());
				gameSession->Send(GameClientPacketHandler::MakeSendBuffer(changePkt));



			}
		}
	}
}
