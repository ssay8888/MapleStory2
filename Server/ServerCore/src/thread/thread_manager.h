#pragma once

#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

public:
	static ThreadManager&  GetInstance()
	{
		static ThreadManager instance;
		return instance;
	}

	auto Launch(std::function<void()> callback) -> void;
	auto Join() -> void;

	static auto InitTls() -> void;
	static auto DestroyTls() -> void;
		   
	static auto DoGlobalQueueWork() -> void;
	static auto DistributeReservedJobs() -> void;

private:
	Mutex						_lock;
	std::vector<std::thread>	_threads;
};

