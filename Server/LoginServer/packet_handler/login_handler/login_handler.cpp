#include "pch.h"
#include "login_handler.h"

#include "login_server.h"
#include "auth_storage/auth_storage.h"
#include "center_server_session/center_login_client_packet_handler.h"
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
	sendPkt.set_result(Protocol::kIdInvalid);
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		/*DBBind<4, 0> bind(*con, L"{CALL dbo.spLogin(?, ?, ?, ?)}");

		std::wstring id = FileUtils::ConvertCtoW(pkt.id().c_str());
		bind.BindParam(0, id.c_str());

		std::wstring pw = FileUtils::ConvertCtoW(pkt.pw().c_str());
		bind.BindParam(1, pw.c_str());

		int32_t outResult;
		bind.BindParamOutput(2, outResult);
		int32_t outResult2;
		bind.BindParamOutput(3, outResult2);
		
		if (bind.Execute())
		{
			while(bind.Fetch())
			{
				int a = 0;
			}
			int b = 0;
		}*/

		DBBind<1, 4> bind(*con, L"{CALL dbo.spGetAccount(?)}");

		std::wstring id = FileUtils::ConvertCtoW(pkt.id().c_str());
		bind.BindParam(0, id.c_str());

		WCHAR account[100]{ 0, };
		bind.BindCol(0, account);

		WCHAR password[100]{ 0, };
		bind.BindCol(1, password);

		int64_t accountid;
		bind.BindCol(2, accountid);

		int8_t loggedin;
		bind.BindCol(3, loggedin);


		if (bind.Execute())
		{
			if (bind.Fetch())
			{
				if (loggedin)
				{
					sendPkt.set_result(Protocol::kAlreadyConnected);
				}
				else
				{
					const auto pktPw = FileUtils::ConvertCtoW(pkt.pw().c_str());
					if (password != pktPw)
					{
						sendPkt.set_result(Protocol::kPwInvlid);
					}
					else if (password == pktPw)
					{

						if (auto con2 = DBConnectionPool::GetInstance().Pop())
						{
							DBBind<1, 0> bind(*con, L"{CALL dbo.spSetLoggedin(?)}");
							bind.BindParam(0, id.c_str());
							if (bind.Execute())
							{
								
							}
							DBConnectionPool::GetInstance().Push(con2);
						}
						sendPkt.set_result(Protocol::kOk);
						loginSession->SetAccountId(accountid);
					}
				}
			}
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
				auto player = MakeShared<Player>(characterId);
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

void LoginHandler::CharacterListSend(const std::shared_ptr<LoginSession> session) const
{
	Protocol::LoginServerCharacterList sendPkt;
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
	session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));
}

void LoginHandler::SelectCharacter(PacketSessionRef session, Protocol::LoginClientCharacterSelect pkt)
{
	auto& authInstance = AuthStorage::GetInstance();
	const auto game_session = std::static_pointer_cast<LoginSession>(session);

	const auto authInfo = authInstance.CreateAuth(game_session);
	Protocol::CenterLoginServerCreateAuth centerSendPkt;
	centerSendPkt.set_auth(authInfo->auth);
	centerSendPkt.set_accountid(game_session->GetAccountId());
	centerSendPkt.set_characterid(pkt.characterid());
	g_login_server->GetCenterServerSevice()->Broadcast(CenterLoginClientPacketHandler::MakeSendBuffer(centerSendPkt));
	authInstance.AddAuth(authInfo);


	//Protocol::LoginServerCharacterSelect sendPkt;
	//session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));

}

#pragma endregion
