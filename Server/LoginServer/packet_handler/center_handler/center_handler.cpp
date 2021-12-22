#include "pch.h"
#include "center_handler.h"

#include "auth_storage/auth_storage.h"
#include "center_server_session/center_login_session.h"
#include "login_session/login_client_packet_handler.h"
#include "login_session/login_session.h"

void CenterHandler::SocketConnectInit(PacketSessionRef session, Protocol::CenterLoginClientInit pkt)
{
	if (pkt.authenticationkey() != 129306812390481)
	{
		session->Disconnect(L"Key Failed");
		return;
	}

	if (const auto center = std::static_pointer_cast<CenterLoginSession>(session))
	{
		center->InitBool();
	}
}

void CenterHandler::CenterLoginClientCheckAuth(PacketSessionRef session, Protocol::CenterLoginClientCheckAuth pkt)
{
	auto cetnerSession = std::static_pointer_cast<CenterLoginSession>(session);
	if (!cetnerSession->IsInit())
	{
		return;
	}

	const auto& authStorage = AuthStorage::GetInstance();

	if (auto auth = authStorage.FindAuth(pkt.accountid()))
	{
		Protocol::LoginServerCharacterSelect sendPkt;
		sendPkt.set_result(Protocol::kCharacterSelectMessage::kSelectSuccess);
		sendPkt.set_auth(pkt.auth());
		sendPkt.set_ip("127.0.0.1");
		sendPkt.set_port(7778);
		auth->session->Send(LoginClientPacketHandler::MakeSendBuffer(sendPkt));
	}
}
