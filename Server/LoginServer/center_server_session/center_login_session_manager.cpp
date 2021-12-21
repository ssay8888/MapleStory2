#include "pch.h"
#include "center_login_session_manaeger.h"
#include "center_login_session.h"

auto CenterLoginSessionManager::Add(const CenterLoginSessionRef& session) -> void
{
	tbb::concurrent_hash_map<int64_t, CenterLoginSessionRef>::accessor result;
	if (_sessions.insert(result, session->GetSessionId()))
	{
		result->second = session;
	}
}

auto CenterLoginSessionManager::Remove(const CenterLoginSessionRef& session) -> void
{
	tbb::concurrent_hash_map<int64_t, CenterLoginSessionRef>::const_accessor result;
	
	if (_sessions.find(result, session->GetSessionId()))
	{
		_sessions.erase(result);
	}
}

auto CenterLoginSessionManager::Broadcast(const SendBufferRef& sendBuffer) -> void
{
	for (const auto& session : _sessions)
	{
		session.second->Send(sendBuffer);
	}
}

auto CenterLoginSessionManager::GetSessionSize() const -> size_t
{
	return _sessions.size();
}
