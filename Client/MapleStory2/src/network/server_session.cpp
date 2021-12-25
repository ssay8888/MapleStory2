#include "c_pch.h"
#include "server_session.h"

#include "game_server_packet_handler.h"
#include "login_server_packet_handler.h"
#include "src/main/main_app.h"
#include "src/network/service.h"

ServerSession::ServerSession():
	_id(0)
{
}

void ServerSession::OnConnected()
{
	if (g_mainApp)
	{
		g_mainApp->SetConnected(true);
	}
}

void ServerSession::OnRecvPacket(BYTE* buffer,  int32_t len)
{
	auto session = GetPacketSessionRef();
	switch (g_service->GetServerType())
	{
	case Service::kServerType::kClientLogin:
		LoginServerPacketHandler::HandlePacket(session, buffer, len);
		break;
	case Service::kServerType::kClientGame:
		GameServerPacketHandler::HandlePacket(session, buffer, len);
		break;
	default: ;
	}
}

void ServerSession::OnSend(int32_t len)
{
}

void ServerSession::OnDisconnected()
{
}
