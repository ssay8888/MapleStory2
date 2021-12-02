#include "pch.h"
#include "game_session_manaeger.h"
#include "game_session.h"

auto GameSessionManager::Add(const GameSessionRef session) -> void
{
	WRITE_LOCK;
	_sessions.insert(session);
}

auto GameSessionManager::Remove(const GameSessionRef session) -> void
{
	WRITE_LOCK;
	_sessions.erase(session);
}

auto GameSessionManager::Broadcast(const SendBufferRef sendBuffer) -> void
{
	WRITE_LOCK;
	for (const GameSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

auto GameSessionManager::GetSessionSize() -> int64_t
{
	READ_LOCK;
	return _sessions.size();
}
