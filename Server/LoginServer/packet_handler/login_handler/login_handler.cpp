#include "pch.h"
#include "login_handler.h"

#include <pugixml.hpp>
#include <sstream>

#include "login_server.h"
#include "auth_storage/auth_storage.h"
#include "center_server_session/center_login_client_packet_handler.h"
#include "data_reader/data_reader_manager.h"
#include "game/player.h"
#include "game/equipped/equipped.h"
#include "login_session/login_client_packet_handler.h"
#include "login_session/login_session.h"
#include "src/database/db_bind.h"
#include "src/database/db_connection_pool.h"
#include "src/network/service.h"
#include "src/utils/file_utils.h"
#include "string_utils/string_utils.h"

#pragma region LoginRequest

void LoginHandler::LoginRequest(const PacketSessionRef session, const Protocol::LoginClientLogin pkt)
{
	const auto loginSession = std::static_pointer_cast<LoginSession>(session);

	Protocol::LoginServerLogin sendPkt;
	sendPkt.set_result(Protocol::kIdError);
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<2, 2> bind(*con, L"{CALL dbo.spLogin(?, ?)}");

		std::wstring id = FileUtils::ConvertCtoW(pkt.id().c_str());
		bind.BindParam(0, id.c_str());

		std::wstring pw = FileUtils::ConvertCtoW(pkt.pw().c_str());
		bind.BindParam(1, pw.c_str());

		int32_t loginResult = 0;
		bind.BindCol(0, loginResult);

		int32_t accountId = 0;
		bind.BindCol(1, accountId);

		if (bind.Execute())
		{
			do 
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0 && bind.Fetch())
				{
					switch (static_cast<kSqlLoginResult>(loginResult))
					{
					case kSqlLoginResult::kIdError:
						sendPkt.set_result(Protocol::kIdError);
						break;
					case kSqlLoginResult::kAlreadyConnected:
						sendPkt.set_result(Protocol::kAlreadyConnected);
						break;
					case kSqlLoginResult::kPasswordError:
						sendPkt.set_result(Protocol::kPasswordError);
						break;
					case kSqlLoginResult::kLoginSuccess:
						sendPkt.set_result(Protocol::kLoginSuccess);
						loginSession->SetAccountId(accountId);
						break;
					default:;
					}
				}
			} while (bind.SqlMoreResults() != SQL_NO_DATA);
		}
		DBConnectionPool::GetInstance().Push(con);
	}

	session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));

}

#pragma endregion



#pragma region CharacterListRequest

void LoginHandler::CharacterListRequest(PacketSessionRef session, Protocol::LoginClientCharacterList pkt)
{
	auto game_session = std::static_pointer_cast<LoginSession>(session);
	if (const auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 6> character_bind(*con, L"{CALL dbo.spAllFittingCharacterInfoLoad(?)}");
		auto accountId = game_session->GetAccountId();
		character_bind.BindParam(0, accountId);

		WCHAR name[100]{ 0, };							 // 캐릭터 정보 시작
		character_bind.BindCol(0, name);
		int64_t characterId;
		character_bind.BindCol(1, characterId);
		int8_t gender;
		character_bind.BindCol(2, gender);
		int32_t face;
		character_bind.BindCol(3, face);

		int32_t itemId;									 // 인벤토리정보시작
		character_bind.BindCol(4, itemId);
		int32_t position;
		character_bind.BindCol(5, position);
		if (character_bind.Execute())
		{
			do
			{
				int16_t count = 0;
				character_bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (character_bind.Fetch())
					{
						auto player = game_session->FindPlayer(characterId);
						if (!player)
						{
							player = MakeShared<Player>(characterId);
							player->SetName(name);
							player->SetGender(gender);
							player->SetFaceId(face);
							game_session->PushPlayer(player);
						}

						player->GetEquipped()->PushItem(itemId);
						player->GetEquipped()->PushItem(player->GetFaceId());
					}
				}
			} while (character_bind.SqlMoreResults() != SQL_NO_DATA);
		}
		CharacterListSend(game_session);

		DBConnectionPool::GetInstance().Push(con);
	}
}

void LoginHandler::CharacterListSend(const std::shared_ptr<LoginSession> session) const
{
	Protocol::LoginServerCharacterList sendPkt;
	const auto playerList = session->GetPlayerList();
	for (const auto& player: playerList)
	{
		const auto character = sendPkt.add_characters();
		character->set_name(StringUtils::ConvertWtoC(player.second->GetName()).c_str());
		character->set_gender(player.second->GetGender());
		character->set_characterid(player.second->GetCharacterId());
		auto items = player.second->GetEquipped()->GetAllItem();
		for (const auto item : items)
		{
			character->add_eqp_items(item);
		}
	}
	session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));
}

#pragma endregion


#pragma region CreateCharacter

void LoginHandler::CreateCharacter(const PacketSessionRef session, const Protocol::LoginClientCreateCharacter pkt)
{
	auto game_session = std::static_pointer_cast<LoginSession>(session);

	Protocol::LoginServerCreateCharacter sendPkt;
	
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<5, 1> bind(*con, L"{CALL dbo.spCreateCharacter(?, ?, ?, ?, ?)}");
		auto accountId = game_session->GetAccountId();
		bind.BindParam(0, accountId);
		auto name = StringUtils::ConvertCtoW(pkt.name().c_str());
		bind.BindParam(1, name.c_str());
		bool gender = static_cast<int8_t>(pkt.gender());
		bind.BindParam(2, gender);
		int32_t face = pkt.faceindex();
		bind.BindParam(3, face);

		auto xml = ItemListToXml(pkt);
		bind.BindParam(4, xml.c_str());

		int result = 0;
		bind.BindCol(0, result);
		if (bind.Execute())
		{
			auto character = sendPkt.mutable_character();
			character->set_name(pkt.name().c_str());
			character->set_gender(gender);
			character->add_eqp_items(face);
			do
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (bind.Fetch())
					{
					}
				}
			} while (bind.SqlMoreResults() != SQL_NO_DATA);

			character->set_characterid(result);
			switch (static_cast<kSqlCreateCharacterResult>(result))
			{
			case kSqlCreateCharacterResult::kIdDuplication:
				sendPkt.set_result(Protocol::kDuplication);
				break;
			case kSqlCreateCharacterResult::kCreateFailed:
				sendPkt.set_result(Protocol::kCreateFailed);
				break;
			default:
				sendPkt.set_result(Protocol::kCreateSuccess);
				break;
			}

			for (int i = 0; i < 3; ++i)
			{
				int itemid = 0;
				switch (i)
				{
				case 0:
				{
					itemid = pkt.coatindex();
					break;
				}
				case 1:
				{
					itemid = pkt.pantsindex();
					break;
				}
				case 2:
				{
					itemid = pkt.shoesindex();
					break;
				}
				}
				if (itemid > 0)
				{
					character->add_eqp_items(itemid);
				}
			}
			character->add_eqp_items(face);
			session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));
			std::cout << "Character Create Success" << std::endl;
		}
		DBConnectionPool::GetInstance().Push(con);
	}
}

std::wstring LoginHandler::ItemListToXml(Protocol::LoginClientCreateCharacter pkt)
{
	using namespace pugi;
	xml_document doc;
	xml_node root = doc.append_child("ms2");

	auto items = root.append_child("items");
	for (int i = 0; i < 3; ++i)
	{
		int itemId = 0;
		switch (i)
		{
		case 0:
			itemId = pkt.coatindex();
			break;
		case 1:
			itemId = pkt.pantsindex();
			break;
		case 2:
			itemId = pkt.shoesindex();
			break;
		}
		if (itemId)
		{
			auto item = items.append_child("item");
			item.append_attribute("type").set_value(0);
			item.append_attribute("itemid").set_value(itemId);
			item.append_attribute("position").set_value(static_cast<int>(GameContents::EquipeType(itemId)));
			auto iteminfo = DataReaderManager::GetInstance().FindItemInfo(itemId);
			if (iteminfo)
			{
				if(iteminfo->constant_id != 0)
				{
					auto option = DataReaderManager::GetInstance().FindItemOptionConstant(iteminfo->constant_id);
					if (option)
					{
						item.append_attribute("str").set_value(option->ranks[L"str"] != nullptr ? option->ranks[L"str"]->value : 0);
						item.append_attribute("dex").set_value(option->ranks[L"dex"] != nullptr ? option->ranks[L"dex"]->value : 0);
						item.append_attribute("_int").set_value(option->ranks[L"int"] != nullptr ? option->ranks[L"int"]->value : 0);
						item.append_attribute("luk").set_value(option->ranks[L"luk"] != nullptr ? option->ranks[L"luk"]->value : 0);
						item.append_attribute("wap").set_value(option->ranks[L"wap"] != nullptr ? option->ranks[L"wap"]->value : 0);
					}
				}
				else
				{
					item.append_attribute("str").set_value(0);
					item.append_attribute("dex").set_value(0);
					item.append_attribute("_int").set_value(0);
					item.append_attribute("luk").set_value(0);
					item.append_attribute("wap").set_value(0);
				}
				//item.append_attribute("quantity").set_value(1);
			}
		}
	}
	std::wstringstream xml;
	doc.save(xml);
	return xml.str();
}
#pragma endregion

#pragma region SelectCharacter
void LoginHandler::SelectCharacter(PacketSessionRef session, Protocol::LoginClientCharacterSelect pkt)
{
	auto& authInstance = AuthStorage::GetInstance();
	const auto game_session = std::static_pointer_cast<LoginSession>(session);

	const auto authInfo = authInstance.CreateAuth(game_session);
	Protocol::CenterLoginServerCreateAuth centerSendPkt;
	std::cout << "CreateAuth" << authInfo->auth;
	centerSendPkt.set_auth(authInfo->auth);
	centerSendPkt.set_accountid(game_session->GetAccountId());
	centerSendPkt.set_characterid(pkt.characterid());
	g_login_server->GetCenterServerSevice()->Broadcast(CenterLoginClientPacketHandler::MakeSendBuffer(centerSendPkt));
	authInstance.RemoveAuth(game_session->GetAccountId());
	authInstance.AddAuth(authInfo);

	//Protocol::LoginServerCharacterSelect sendPkt;
	//session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));

}


#pragma endregion