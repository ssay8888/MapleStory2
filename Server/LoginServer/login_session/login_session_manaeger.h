#pragma once

class LoginSession;

using LoginSessionRef = std::shared_ptr<LoginSession>;

class LoginSessionManager
{

public:
	static auto GetInstance() -> LoginSessionManager&
	{
		static LoginSessionManager instance;
		return instance;
	}
	auto Add(const LoginSessionRef& session) -> void;
	auto Remove(const LoginSessionRef& session) -> void;
	auto Broadcast(const SendBufferRef& sendBuffer) -> void;
	auto GetSessionSize() const ->size_t;

private:
	tbb::concurrent_hash_map<int64_t, LoginSessionRef> _sessions;
};
