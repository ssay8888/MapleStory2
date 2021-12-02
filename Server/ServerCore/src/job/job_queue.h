#pragma once
#include "job.h"
#include "src/utils/lock_queue.h"
#include "src/job/job_timer.h"
#include "src/memory/object_pool.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	auto DoAsync(CallbackType&& callback) -> void;

	template<typename T, typename Ret, typename... Args>
	auto DoAsync(Ret(T::*memFunc)(Args...), Args... args) -> void;

	auto DoTimer(uint64_t tickAfter, CallbackType&& callback) -> void;

	template<typename T, typename Ret, typename... Args>
	auto DoTimer(uint64_t tickAfter, Ret(T::* memFunc)(Args...), Args... args) -> void;

	auto ClearJobs() -> void;

public:
	auto Push(JobRef job, bool pushOnly = true) -> void;
	auto Execute() -> void;

protected:
	LockQueue<JobRef>		_jobs;
	Atomic<int32_t>			_job_count = 0;
};

template <typename T, typename Ret, typename ... Args>
auto JobQueue::DoAsync(Ret(T::* memFunc)(Args...), Args... args) -> void
{
	std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
	Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...), true);
}

template <typename T, typename Ret, typename ... Args>
auto JobQueue::DoTimer(uint64_t tickAfter, Ret(T::* memFunc)(Args...), Args... args) -> void
{
	std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
	const JobRef job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
	JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
}

