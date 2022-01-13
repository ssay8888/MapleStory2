#pragma once
#include <tbb/concurrent_queue.h>

#include "protocol/game_protocol.pb.h"

class MapInstance;

class GameTick : public std::enable_shared_from_this<GameTick>
{

public:
	static auto GetInstance()->std::shared_ptr<GameTick>
	{
		static std::shared_ptr<GameTick> instance = std::make_shared<GameTick>();
		return instance;
	}

	auto GameLoopInit()->void;

	template<typename T, typename Ret, typename... Args>
	auto DoAsync(Ret(T::* memFunc)(Args...), Args... args) -> void;

public:
	auto AddCharcter(std::shared_ptr<MapInstance> mapInstance, GameSessionRef gameSession)->void;
	auto BroadCastAddCharacter(std::shared_ptr<MapInstance> mapInstance, GameSessionRef gameSession)->void;
	auto RemoveCharacter(std::shared_ptr<MapInstance> mapInstance, int64_t characterId)->void;

	auto TakeDamage(int64_t characterId, int64_t monsterObjectId, GameSessionRef gameSession)->void;
	auto AttackMonster(Protocol::GameClientAttackMonster pkt, GameSessionRef gameSession)->void;

private:
	std::atomic<bool> _exit_loop;
	
	tbb::concurrent_queue<JobRef> _jobs;

};

template <typename T, typename Ret, typename ... Args>
auto GameTick::DoAsync(Ret(T::* memFunc)(Args...), Args... args) -> void
{
	std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
	_jobs.push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
}
