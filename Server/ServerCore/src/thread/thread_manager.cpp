#include "pch.h"
#include "thread_manager.h"
#include "core_tls.h"
#include "src/job/global_queue.h"

ThreadManager::ThreadManager()
{
	InitTls();
}

ThreadManager::~ThreadManager()
{
	Join();
}

auto ThreadManager::Launch(std::function<void(void)> callback) -> void
{
	LockGuard guard(_lock);

	_threads.push_back(std::thread([=]()
		{
			InitTls();
			callback();
			DestroyTls();
		}));
}

auto ThreadManager::Join() -> void
{
	for (auto& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

auto ThreadManager::InitTls() -> void
{
	static Atomic<uint32_t> sThreadId = 1;
	LThreadId = sThreadId.fetch_add(1);
}

auto ThreadManager::DestroyTls() -> void
{

}

auto ThreadManager::DoGlobalQueueWork() -> void
{
	while (true)
	{
		const uint64_t now = GetTickCount64();
		if (now > LEndTickCount)
			break;

		JobQueueRef jobQueue = GlobalQueue::GetInstance().Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}

auto ThreadManager::DistributeReservedJobs() -> void
{
	const uint64_t now = GetTickCount64();

	JobTimer::GetInstance().Distribute(now);
}
