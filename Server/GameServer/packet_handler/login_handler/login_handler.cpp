#include "pch.h"
#include "login_handler.h"

#include "game_session/client_packet_handler.h"
#include "src/database/db_bind.h"
#include "src/database/db_connection_pool.h"
#include "src/utils/file_utils.h"

void LoginHandler::LoginRequest(PacketSessionRef session, Protocol::ClientLogin pkt)
{
	Protocol::ServerLogin sendPkt;
	sendPkt.set_result(Protocol::kIdInvalid);
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 2> bind(*con, L"{CALL dbo.spGetAccount(?)}");

		std::wstring id = FileUtils::ConvertCtoW(pkt.id().c_str());
		bind.BindParam(0, L"asdf");

		WCHAR account[100]{0,};
		WCHAR password[100]{0,};
		bind.BindCol(0, account);
		bind.BindCol(1, password);
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
			}
		}
		DBConnectionPool::GetInstance().Push(con);
	}
	
	session->Send(ClientPacketHandler::MakeSendBuffer(sendPkt));

}
