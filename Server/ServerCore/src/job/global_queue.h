#pragma once

class GlobalQueue
{
private:
	GlobalQueue();
	~GlobalQueue();

public:
	static GlobalQueue& GetInstance()
	{
		static GlobalQueue instance;
		return instance;
	}

	auto Push(JobQueueRef jobQueue) -> void;
	auto Pop() -> JobQueueRef;

private:
	LockQueue<JobQueueRef> _job_queues;
};

