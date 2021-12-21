#include "game_server_pch.h"
#include "center_client_session.h"

#include "center_login_server_packet_handler.h"
#include "src/network/service.h"

static std::atomic<int> g_session_id = 1;

CenterClientSession::CenterClientSession() :
	_session_id(g_session_id.fetch_add(1))
{
}

void CenterClientSession::OnConnected()
{
	std::cout << "센터서버 접속" << std::endl;
	auto game_session = std::static_pointer_cast<GameSession>(shared_from_this());

	switch (GetService()->GetServerType())
	{
	case Service::kServerType::kCenterClientGame:
		ConnectedCenterServerInit();
		break;
	default:;
	}
}

void CenterClientSession::OnDisconnected()
{

}

void CenterClientSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	PacketSessionRef session = GetPacketSessionRef();
	switch (GetService()->GetServerType())
	{
	case Service::kServerType::kCenterClientGame:
		CenterLoginServerPacketHandler::HandlePacket(session, buffer, len);
		break;
	default:;
	}
}

void CenterClientSession::OnSend(int32_t len)
{

}

auto CenterClientSession::ConnectedCenterServerInit() -> void
{
	Protocol::CenterLoginClientInit sendPkt;
	sendPkt.set_authenticationkey(129306812390481);
	this->Send(CenterLoginServerPacketHandler::MakeSendBuffer(sendPkt));
}

auto CenterClientSession::GetSessionId() const -> int64_t
{
	return _session_id;
}
