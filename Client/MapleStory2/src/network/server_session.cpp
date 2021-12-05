#include "c_pch.h"
#include "server_session.h"

#include "server_packet_handler.h"
#include "src/main/main_app.h"

ServerSession::ServerSession()
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
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int32_t len)
{
}

void ServerSession::OnDisconnected()
{
}
