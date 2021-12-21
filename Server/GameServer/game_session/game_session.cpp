#include "game_server_pch.h"
#include "game_session.h"

#include "game_client_packet_handler.h"
#include "game_session_manager.h"

static std::atomic<int> g_session_id = 1;

GameSession::GameSession() :
	_session_id(g_session_id.fetch_add(1))
{
}

auto GameSession::OnConnected() -> void
{
	std::cout << "Á¢¼Ó" << std::endl;
	auto game_session = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().GameSessionAdd(game_session);

}

auto GameSession::OnDisconnected() -> void
{
	auto packetSession = std::static_pointer_cast<PacketSession>(shared_from_this());
	auto gameSession = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().GameSessionRemove(gameSession);

}

auto GameSession::OnRecvPacket(BYTE* buffer, const int32_t len) -> void
{
	PacketSessionRef session = GetPacketSessionRef();
	auto header = reinterpret_cast<PacketHeader*>(buffer);
	GameClientPacketHandler::HandlePacket(session, buffer, len);
}

auto GameSession::OnSend(int32_t len) -> void
{
}

auto GameSession::GetSessionId() const -> int64_t
{
	return _session_id;
}

auto GameSession::GetAccountId() const -> int64_t
{
	return _account_id;
}

auto GameSession::SetAccountId(const int64_t id) -> void
{
	_account_id = id;
}

auto GameSession::GetPlayer() const -> std::shared_ptr<Player>
{
	return _player;
}
