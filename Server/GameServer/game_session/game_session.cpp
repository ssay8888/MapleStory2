#include "pch.h"
#include "game_session.h"
#include "game_session_manaeger.h"
#include "client_packet_handler.h"
#include "player/player.h"
#include "src/utils/buffer_writer.h"

static std::atomic<int> g_session_id = 1;

GameSession::GameSession() :
	_session_id(g_session_id.fetch_add(1))
{
}

auto GameSession::OnConnected() -> void
{
	std::cout << "접속" << std::endl;
	auto game_session = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().Add(game_session);
	
	_current_player = MakeShared<Player>(game_session);
}

auto GameSession::OnDisconnected() -> void
{
	auto packetSession = std::static_pointer_cast<PacketSession>(shared_from_this());
	auto gameSession = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().Remove(gameSession);

	_current_player = nullptr;
}

auto GameSession::OnRecvPacket(BYTE* buffer, const int32_t len) -> void
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

auto GameSession::OnSend(int32_t len) -> void
{
}

auto GameSession::GetPlayer() const -> std::shared_ptr<Player>
{
	return _current_player;
}

auto GameSession::GetSessionId() const -> int64_t
{
	return _session_id;
}
