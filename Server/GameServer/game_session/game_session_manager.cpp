#include "game_server_pch.h"
#include "game_session_manager.h"

auto GameSessionManager::GameSessionAdd(const GameSessionRef& session) -> void
{
	//_sessions.emplace(session, session);
}

auto GameSessionManager::GameSessionRemove(const GameSessionRef& session) -> void
{
	//_sessions.erase(session);
}

auto GameSessionManager::Broadcast(const SendBufferRef sendBuffer) -> void
{
	for (const auto& session : _sessions)
	{
		session.second->Send(sendBuffer);
	}
}

auto GameSessionManager::GetGameSessionSize() const -> size_t
{
	//return _sessions.size();
	return 0;
}
