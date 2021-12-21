#pragma once
class GameSession;

class GameSessionManager
{

public:
	static auto GetInstance() -> GameSessionManager&
	{
		static GameSessionManager instance;
		return instance;
	}

	auto GameSessionAdd(const GameSessionRef& session) -> void;
	auto GameSessionRemove(const GameSessionRef& session) -> void;
	auto Broadcast(SendBufferRef sendBuffer) -> void;
	auto GetGameSessionSize() const ->size_t;

private:
	tbb::concurrent_hash_map<GameSessionRef, GameSessionRef> _sessions;
};
