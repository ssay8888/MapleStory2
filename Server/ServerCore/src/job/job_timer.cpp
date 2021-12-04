#include "pch.h"
#include "job_timer.h"
#include "job_queue.h"

JobData::JobData(const std::weak_ptr<JobQueue> owner, const JobRef job)
	: owner(owner),
      job(job)
{

}

auto TimerItem::operator<(const TimerItem& other) const -> bool
{
	return execute_tick > other.execute_tick;
}

auto JobTimer::Reserve(const uint64_t tickAfter, std::weak_ptr<JobQueue> owner, JobRef job) -> void 
{
	const uint64_t executeTick = GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick, jobData });
}

auto JobTimer::Distribute(const uint64_t now) -> void
{
	// 한 번에 1 쓰레드만 통과
	if (_distributing.exchange(true) == true)
		return;

	Vector<TimerItem> items;

	{
		WRITE_LOCK;

		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.execute_tick)
				break;

			items.push_back(timerItem);
			_items.pop();
		}
	}

	for (const TimerItem& item : items)
	{
		if (const JobQueueRef owner = item.job_data->owner.lock())
			owner->Push(item.job_data->job);

		ObjectPool<JobData>::Push(item.job_data);
	}

	_distributing.store(false);
}

auto JobTimer::Clear() -> void
{
	WRITE_LOCK;

	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		ObjectPool<JobData>::Push(timerItem.job_data);
		_items.pop();
	}
}
