#include "pch.h"
#include "center_handler.h"

#include "center_server_session/center_login_session.h"

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
