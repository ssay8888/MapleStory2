#include "game_server_pch.h"
#include "game_character_load_queue.h"

#include <src/database/db_bind.h>

#include "game/entitiy/character/game_character.h"
#include "game/entitiy/character/information_collection/inventorys/inventorys.h"
#include "game/entitiy/character/information_collection/stats/stats.h"
#include "game_session/game_client_packet_handler.h"
#include "managers/auth_manager/game_auth_manager.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "src/database/db_connection_pool.h"
#include "string_utils/string_utils.h"

auto GameCharacterLoadQueue::GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt) -> void
{
	//처음 유저가 로딩이 끝나고 캐릭터의 정보를 로드하고 보내준다.
	auto authManager = GameAuthManager::GetInstance();
	auto authInfo = authManager.FindAuth(pkt.auth());

	Protocol::GameServerLoadCharacter sendPkt;
	sendPkt.set_state(Protocol::kLoadFailed);
	if (authInfo)
	{
		auto character = GameCharacter::Create(authInfo->characterid());
		SettingCharacterInfoSendPacket(sendPkt, session, character);
		session->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt));
		return;
	}
	session->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt));
}

auto GameCharacterLoadQueue::SettingCharacterInfoSendPacket(Protocol::GameServerLoadCharacter& sendPkt, PacketSessionRef session, std::shared_ptr<GameCharacter> player) -> void
{
	sendPkt.set_state(Protocol::kLoadSuccess);
	sendPkt.set_account_id(player->GetAccountId());
	sendPkt.set_character_id(player->GetCharacterId());
	sendPkt.set_name(StringUtils::ConvertWtoC(player->GetName().c_str()));
	sendPkt.set_gender(player->GetGender());
	sendPkt.set_face_id(player->GetFaceId());
	auto& InfoManager = CharacterInfoStorageManager::GetInstance();
	auto baseStatInfo = InfoManager.FindInfo(CharacterInfoStorage::kInfoTypes::kStats, player->GetCharacterId());
	if (baseStatInfo)
	{
		auto statInfo = std::static_pointer_cast<Stats>(baseStatInfo);
		sendPkt.set_str(statInfo->GetStr());
		sendPkt.set_dex(statInfo->GetDex());
		sendPkt.set_int_(statInfo->GetInt());
		sendPkt.set_luk(statInfo->GetLuk());
	}
	auto baseInventoryInfo = InfoManager.FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, player->GetCharacterId());
	if (baseInventoryInfo)
	{
		const auto statInfo = std::static_pointer_cast<Inventorys>(baseInventoryInfo);
		const auto allItems = statInfo->AllItems();
		for (auto itemPair : allItems)
		{
			auto item = sendPkt.add_items();
			item->set_position(itemPair.first);
			item->set_itemid(itemPair.first);
		}
	}
}
