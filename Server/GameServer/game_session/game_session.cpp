#include "game_server_pch.h"
#include "game_session.h"

#include "game_client_packet_handler.h"
#include "game_session_manager.h"
#include "game/entitiy/character/game_character.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "game_tick/game_tick.h"
#include "managers/character_info_manager/character_info_storage_manager.h"

static std::atomic<int> g_session_id = 1;

GameSession::GameSession() :
	_session_id(g_session_id.fetch_add(1))
{
	std::cout << _session_id << "생성" << std::endl;
}

auto GameSession::OnConnected() -> void
{
	std::cout << "접속" << std::endl;
	const auto game_session = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().GameSessionAdd(game_session);
}

auto GameSession::OnDisconnected() -> void
{
	auto packetSession = std::static_pointer_cast<PacketSession>(shared_from_this());
	const auto gameSession = std::static_pointer_cast<GameSession>(shared_from_this());
	GameSessionManager::GetInstance().GameSessionRemove(gameSession);

	const auto mapInstance = MapManager::GetInstance().FindMapInstance(_character->GetMapId());
	GameTick::GetInstance()->DoAsync(&GameTick::RemoveCharacter, mapInstance, _character->GetCharacterId());

	const auto& InfoStorageManager = CharacterInfoStorageManager::GetInstance();
	auto result = InfoStorageManager.RemoveAllInfo(_character->GetCharacterId());

}

auto GameSession::OnRecvPacket(BYTE* buffer, const int32_t len) -> void
{
	PacketSessionRef session = GetPacketSessionRef();
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

auto GameSession::SetPlayer(std::shared_ptr<GameCharacter> character) -> void
{
	_character = character;
}

auto GameSession::GetPlayer() const -> std::shared_ptr<GameCharacter>
{
	return _character;
}
