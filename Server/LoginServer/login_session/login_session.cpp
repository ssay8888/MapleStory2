#include "pch.h"
#include "login_session.h"
#include "login_session_manaeger.h"
#include "login_client_packet_handler.h"
#include "game/loggedin/login.h"
#include "src/utils/buffer_writer.h"

static std::atomic<int> g_session_id = 1;

LoginSession::LoginSession() :
	_session_id(g_session_id.fetch_add(1))
{
}

auto LoginSession::OnConnected() -> void
{
	std::cout << "접속" << std::endl;
	auto game_session = std::static_pointer_cast<LoginSession>(shared_from_this());
	LoginSessionManager::GetInstance().Add(game_session);

	_login = MakeShared<Login>(game_session);
}

auto LoginSession::OnDisconnected() -> void
{
	auto packetSession = std::static_pointer_cast<PacketSession>(shared_from_this());
	auto gameSession = std::static_pointer_cast<LoginSession>(shared_from_this());
	LoginSessionManager::GetInstance().Remove(gameSession);
	_login->Disconnected();

}

auto LoginSession::OnRecvPacket(BYTE* buffer, const int32_t len) -> void
{
	PacketSessionRef session = GetPacketSessionRef();
	auto header = reinterpret_cast<PacketHeader*>(buffer);
	LoginClientPacketHandler::HandlePacket(session, buffer, len);
}

auto LoginSession::OnSend(int32_t len) -> void
{
}

auto LoginSession::GetSessionId() const -> int64_t
{
	return _session_id;
}

auto LoginSession::GetAccountId() const -> int64_t
{
	return _account_id;
}

auto LoginSession::SetAccountId(const int64_t id) -> void
{
	_account_id = id;
}

auto LoginSession::PushPlayer(const std::shared_ptr<Player> player)->void
{
	_players.push_back(player);
}

auto LoginSession::GetPlayerList() const -> std::vector<std::shared_ptr<Player>>
{
	return _players;
}
