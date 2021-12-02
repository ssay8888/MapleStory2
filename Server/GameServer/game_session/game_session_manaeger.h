#pragma once

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{

public:
	static auto GetInstance() -> GameSessionManager&
	{
		static GameSessionManager instance;
		return instance;
	}
	auto Add(GameSessionRef session) -> void;
	auto Remove(GameSessionRef session) -> void;
	auto Broadcast(SendBufferRef sendBuffer) -> void;
	auto GetSessionSize() ->int64_t;

private:
	int a = 0;
	USE_LOCK;
	Set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;
