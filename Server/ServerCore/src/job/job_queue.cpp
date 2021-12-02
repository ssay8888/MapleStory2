#include "pch.h"
#include "job_queue.h"
#include "global_queue.h"

auto JobQueue::DoAsync(CallbackType&& callback) -> void
{
	Push(ObjectPool<Job>::MakeShared(std::move(callback)));
}

auto JobQueue::DoTimer(uint64_t tickAfter, CallbackType&& callback) -> void
{
	JobRef job = ObjectPool<Job>::MakeShared(std::move(callback));
	JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
}

auto JobQueue::ClearJobs() -> void
{
	_jobs.Clear();
}

auto JobQueue::Push(const JobRef job, const bool pushOnly) -> void
{
	const int32_t prevCount = _job_count.fetch_add(1);
	_jobs.Push(job); 

	if (prevCount == 0)
	{
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			GlobalQueue::GetInstance().Push(shared_from_this());
		}
	}
}

auto JobQueue::Execute() -> void
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32_t jobCount = static_cast<int32_t>(jobs.size());
		for (int32_t i = 0; i < jobCount; i++)
			jobs[i]->Execute();
		
		if (_job_count.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64_t now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			GlobalQueue::GetInstance().Push(shared_from_this());
			break;
		}
	}
}
