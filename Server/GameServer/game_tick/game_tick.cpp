#include "game_server_pch.h"
#include "game_tick.h"

#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "src/thread/thread_manager.h"

auto GameTick::GameLoopInit() -> void
{
	auto& threadManager = ThreadManager::GetInstance();

	threadManager.Launch([this]()
		{
			uint64_t StartTime = 0;
			const auto mapInstances = MapManager::GetInstance().AllMapInstance();
			while (!_exit_loop)
			{
				if (StartTime + 200 < GetTickCount64())
				{
					StartTime = GetTickCount64();
					for (const auto mapInstance : mapInstances)
					{
						mapInstance->Respawn();
					}
				}
				JobRef job;
				if (_jobs.empty() == false)
				{
					while (_jobs.try_pop(job))
					{
						job->Execute();
					}
				}
			}
		});
}
