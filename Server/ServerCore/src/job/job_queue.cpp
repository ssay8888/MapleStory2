#include "pch.h"
#include "job_queue.h"
#include "global_queue.h"

auto JobQueue::DoAsync(CallbackType&& callback) -> void
{
	Push(ObjectPool<Job>::MakeShared(std::move(callback)));
}

auto JobQueue::DoTimer(uint64 tickAfter, CallbackType&& callback) -> void
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
	const int32 prevCount = _job_count.fetch_add(1);
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

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();
		
		if (_job_count.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			GlobalQueue::GetInstance().Push(shared_from_this());
			break;
		}
	}
}
