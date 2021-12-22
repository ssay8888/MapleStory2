#include "pch.h"
#include "center_login_session.h"

#include "center_login_client_packet_handler.h"
#include "center_login_session_manaeger.h"
#include "game/loggedin/login.h"
#include "src/network/service.h"

static std::atomic<int> g_session_id = 1;

CenterLoginSession::CenterLoginSession() :
	_session_id(g_session_id.fetch_add(1))
{
}

auto CenterLoginSession::OnConnected() -> void
{
	std::cout << "센터서버 접속" << std::endl;
	auto game_session = std::static_pointer_cast<CenterLoginSession>(shared_from_this());
	CenterLoginSessionManager::GetInstance().Add(game_session);
	

}

auto CenterLoginSession::OnDisconnected() -> void
{
	auto packetSession = std::static_pointer_cast<PacketSession>(shared_from_this());
	auto gameSession = std::static_pointer_cast<CenterLoginSession>(shared_from_this());
	CenterLoginSessionManager::GetInstance().Remove(gameSession);
}

auto CenterLoginSession::OnRecvPacket(BYTE* buffer, const int32_t len) -> void
{
	PacketSessionRef session = GetPacketSessionRef();
	switch (GetService()->GetServerType())
	{
	case Service::kServerType::kCenterServerLogin:
		CenterLoginClientPacketHandler::HandlePacket(session, buffer, len);
		break;
	default: ;
	}
}

auto CenterLoginSession::OnSend(int32_t len) -> void
{
}

auto CenterLoginSession::GetSessionId() const -> int64_t
{
	return _session_id;
}

auto CenterLoginSession::InitBool()->void
{
	_init = true;
}

auto CenterLoginSession::IsInit() const -> bool
{
	return _init;
}
