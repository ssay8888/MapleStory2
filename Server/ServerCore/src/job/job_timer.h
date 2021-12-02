#pragma once

struct JobData
{
	JobData(std::weak_ptr<JobQueue> owner, JobRef job);

	std::weak_ptr<JobQueue>	owner;
	JobRef				job;
};

struct TimerItem
{
	auto operator<(const TimerItem& other) const -> bool;

	uint64_t execute_tick = 0;
	JobData* job_data = nullptr;
};

class JobTimer
{
private:
	JobTimer() = default;
	~JobTimer() = default;
public:

	static auto GetInstance() -> JobTimer&
	{
		static JobTimer instance;
		return instance;
	}

	void			Reserve(uint64_t tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void			Distribute(uint64_t now);
	void			Clear();

private:
	USE_LOCK;
	PriorityQueue<TimerItem>	_items;
	Atomic<bool>				_distributing = false;
};

