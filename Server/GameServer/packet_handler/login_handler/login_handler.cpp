#include "pch.h"
#include "login_handler.h"

#include "game/player.h"
#include "game/equipped/equipped.h"
#include "game_session/client_packet_handler.h"
#include "game_session/game_session.h"
#include "src/database/db_bind.h"
#include "src/database/db_connection_pool.h"
#include "src/utils/file_utils.h"
#include "string_utils/string_utils.h"

#pragma region LoginRequest

void LoginHandler::LoginRequest(const PacketSessionRef session, const Protocol::ClientLogin pkt)
{
	auto game_session = std::static_pointer_cast<GameSession>(session);

	Protocol::ServerLogin sendPkt;
	sendPkt.set_result(Protocol::kIdInvalid);
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 3> bind(*con, L"{CALL dbo.spGetAccount(?)}");

		std::wstring id = FileUtils::ConvertCtoW(pkt.id().c_str());
		bind.BindParam(0, L"asdf");

		WCHAR account[100]{ 0, };
		WCHAR password[100]{ 0, };
		int64_t accountid;
		bind.BindCol(0, account);
		bind.BindCol(1, password);
		bind.BindCol(2, accountid);
		bind.Execute();
		if (bind.Fetch())
		{
			auto pkt_pw = FileUtils::ConvertCtoW(pkt.pw().c_str());
			if (password != pkt_pw)
			{
				sendPkt.set_result(Protocol::kPwInvlid);
			}
			else if (password == pkt_pw)
			{
				sendPkt.set_result(Protocol::kOk);
				game_session->SetAccountId(accountid);
			}
		}
		DBConnectionPool::GetInstance().Push(con);
	}

	session->Send(ClientPacketHandler::MakeSendBuffer(sendPkt));

}

#pragma endregion



#pragma region CharacterListRequest

void LoginHandler::CharacterListRequest(PacketSessionRef session, Protocol::ClientCharacterList pkt)
{
	auto game_session = std::static_pointer_cast<GameSession>(session);
	if (const auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 4> character_bind(*con, L"{CALL dbo.spLoadAllCharacter(?)}");
		auto accountId = game_session->GetAccountId();
		character_bind.BindParam(0, accountId);


		WCHAR name[100]{ 0, };
		character_bind.BindCol(0, name);
		int64_t characterId;
		character_bind.BindCol(1, characterId);
		int8_t gender;
		character_bind.BindCol(2, gender);
		int32_t face;
		character_bind.BindCol(3, face);
		if (character_bind.Execute())
		{
			while (character_bind.Fetch())
			{
				auto player = std::make_shared<Player>(characterId);
				player->SetName(name);
				player->SetGender(gender);
				player->SetFaceId(face);
				game_session->PushPlayer(player);
			}
		}

		const auto players = game_session->GetPlayerList();
		for (const auto& player : players)
		{
			DBBind<1, 2> item_bind(*con, L"{CALL dbo.spLoadAllIEqpItem(?)}");
			auto playerId = player->GetCharacterId();
			item_bind.BindParam(0, playerId);

			int32_t itemId;
			item_bind.BindCol(0, itemId);
			int32_t position;
			item_bind.BindCol(1, position);
			if (item_bind.Execute())
			{
				while (item_bind.Fetch())
				{
					player->GetEquipped()->PushItem(itemId);
				}
				player->GetEquipped()->PushItem(player->GetFaceId());
			}
		}

		CharacterListSend(game_session);

		DBConnectionPool::GetInstance().Push(con);
	}
}

void LoginHandler::CharacterListSend(const std::shared_ptr<GameSession> session) const
{
	Protocol::ServerCharacterList sendPkt;
	const auto playerList = session->GetPlayerList();
	for (const auto& player: playerList)
	{
		const auto character = sendPkt.add_characters();
		character->set_name(StringUtils::ConvertWtoC(player->GetName()).c_str());
		character->set_gender(player->GetGender());
		auto items = player->GetEquipped()->GetAllItem();
		for (const auto item : items)
		{
			character->add_eqp_items(item);
		}
	}
	session->Send(ClientPacketHandler::MakeSendBuffer(sendPkt));
}

#pragma endregion


#pragma region CreateCharacter

void LoginHandler::CreateCharacter(const PacketSessionRef session, const Protocol::ClientCreateCharacter pkt)
{
	auto game_session = std::static_pointer_cast<GameSession>(session);

	Protocol::ServerCreateCharacter sendPkt;
	
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<6, 0> bind(*con, L"{CALL dbo.spInsertCharacter(?, ?, ?, ?, ?, ?)}");
		auto accountId = game_session->GetAccountId();
		bind.BindParam(0, accountId);
		auto name = StringUtils::ConvertCtoW(pkt.name().c_str());
		bind.BindParam(1, name.c_str());
		bool gender = static_cast<int8_t>(pkt.gender());
		bind.BindParam(2, gender);
		int32_t face = pkt.faceindex();
		bind.BindParam(3, face);

		int32_t identity = 0;
		bind.BindParamOutput(4, identity);
		int32_t result = 0;
		bind.BindParamOutput(5, result);
		

		if (bind.Execute())
		{
			while (bind.Fetch())
			{
			}
			if (result >= 1)
			{
				std::cout << "Character Create Failed ErrorCode : " << result << std::endl;
				sendPkt.set_result(Protocol::kDuplication);
			}
			else
			{

				auto character = sendPkt.mutable_character();
				DBBind<1, 1> bind(*con, L"{CALL dbo.spGetCharacter(?)}");

				bind.BindParam(0, name.c_str());
				character->set_name(StringUtils::ConvertWtoC(name.c_str()));
				character->set_gender(gender);
				int64_t id;
				bind.BindCol(0, id);
				if (bind.Execute())
				{
					bind.Fetch();

					for (int i = 0; i < 4; ++i)
					{

						if (auto con2 = DBConnectionPool::GetInstance().Pop())
						{
							DBBind<3, 0> createItem(*con2, L"{CALL dbo.spInsertItem(?, ?, ?)}");
							createItem.BindParam(0, id);
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
							createItem.BindParam(1, itemid);
							auto type = static_cast<int32_t>(GameContents::EquipeType(itemid));
							createItem.BindParam(2, type);
							if (itemid > 0)
							{
								if (createItem.Execute())
								{
									character->add_eqp_items(itemid);
								}
							}
							DBConnectionPool::GetInstance().Push(con2);
						}

					}
				}


				std::cout << "Character Create Success" << std::endl;
				character->add_eqp_items(face);
				sendPkt.set_result(Protocol::kCreateSuccess);
			}
		}
		DBConnectionPool::GetInstance().Push(con);
	}
	session->Send(ClientPacketHandler::MakeSendBuffer(sendPkt));
}

#pragma endregion
