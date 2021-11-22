#include "pch.h"
#include "global_queue.h"

GlobalQueue::GlobalQueue()
{

}

GlobalQueue::~GlobalQueue()
{

}

auto GlobalQueue::Push(const JobQueueRef jobQueue) -> void
{
	_job_queues.Push(jobQueue);
}

auto GlobalQueue::Pop() -> JobQueueRef
{
	return _job_queues.Pop();
}
