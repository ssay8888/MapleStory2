#pragma once

class CenterLoginSession;

using CenterLoginSessionRef = std::shared_ptr<CenterLoginSession>;

class CenterLoginSessionManager
{

public:
	static auto GetInstance() -> CenterLoginSessionManager&
	{
		static CenterLoginSessionManager instance;
		return instance;
	}
	auto Add(const CenterLoginSessionRef& session) -> void;
	auto Remove(const CenterLoginSessionRef& session) -> void;
	auto Broadcast(const SendBufferRef& sendBuffer) -> void;
	auto GetSessionSize() const ->size_t;

private:
	tbb::concurrent_hash_map<int64_t, CenterLoginSessionRef> _sessions;
};
