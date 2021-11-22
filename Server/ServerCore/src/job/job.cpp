#include "pch.h"
#include "job.h"

Job::Job(CallbackType&& callback)
	: _callback(std::move(callback))
{
}

void Job::Execute() const
{
	_callback();
}
