#include "game_server_pch.h"
#include "game_tick.h"

#include "game/entitiy/monster/game_monster.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "src/thread/thread_manager.h"
#include "src/utility/timers/timer_manager.h"

auto GameTick::GameLoopInit() -> void
{
	auto& threadManager = ThreadManager::GetInstance();

	auto& timerManager = TimerManager::GetInstance();
	timerManager.AddTimers(TEXT("Timer_Default"));
	timerManager.AddTimers(TEXT("Timer_JobQueue"));

	threadManager.Launch([this]()
		{
			auto& timerManager = TimerManager::GetInstance();
			const auto mapInstances = MapManager::GetInstance().AllMapInstance();

			float timerAcc = 0.f;
			while (!_exit_loop)
			{
				const float	timeDelta = timerManager.ComputeTimeDelta(TEXT("Timer_Default"));
				timerAcc += timeDelta;
				if (timerAcc > 0.2f)
				{
					for (const auto mapInstance : mapInstances)
					{
						mapInstance->Respawn();
						const auto& monsters = mapInstance->GetMonsters();
						for (const auto& monster : monsters)
						{
							monster.second->Tick(timerAcc);
						}
						for (const auto& monster : monsters)
						{
							monster.second->LateTick(timerAcc);
						}
					}
					timerAcc = 0.f;
				}
				if (_jobs.empty() == false)
				{
					JobRef job;
					const uint64_t startTime = GetTickCount64();
					while (_jobs.try_pop(job))
					{
						job->Execute();
						if (startTime + 100 < GetTickCount64())
						{
							break;
						}
					}
				}
			}
			int a = 0;;
		});
}

auto GameTick::AddCharcter(std::shared_ptr<MapInstance> mapInstance, GameSessionRef gameSession)->void
{
	mapInstance->AddCharacter(gameSession);
	mapInstance->RespawnAllMonster(gameSession);
}

auto GameTick::BroadCastAddCharacter(std::shared_ptr<MapInstance> mapInstance, GameSessionRef gameSession) -> void
{
	mapInstance->BroadCastAddCharacter(gameSession);
}

auto GameTick::RemoveCharacter(std::shared_ptr<MapInstance> mapInstance, int64_t characterId) -> void
{
	mapInstance->RemoveCharacter(characterId);
}
