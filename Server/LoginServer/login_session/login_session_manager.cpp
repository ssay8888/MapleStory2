#include "pch.h"
#include "login_session_manaeger.h"
#include "login_session.h"

auto LoginSessionManager::Add(const LoginSessionRef& session) -> void
{
	tbb::concurrent_hash_map<int64_t, LoginSessionRef>::accessor result;
	if (_sessions.insert(result, session->GetSessionId()))
	{
		result->second = session;
	}
}

auto LoginSessionManager::Remove(const LoginSessionRef& session) -> void
{
	tbb::concurrent_hash_map<int64_t, LoginSessionRef>::const_accessor result;
	
	if (_sessions.find(result, session->GetSessionId()))
	{
		_sessions.erase(result);
	}
}

auto LoginSessionManager::Broadcast(const SendBufferRef& sendBuffer) -> void
{
	for (const auto& session : _sessions)
	{
		session.second->Send(sendBuffer);
	}
}

auto LoginSessionManager::GetSessionSize() const -> size_t
{
	return _sessions.size();
}
