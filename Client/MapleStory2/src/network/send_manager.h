#pragma once
#include <concurrent_queue.h>

class SendManager
{
private:
	SendManager() = default;
	~SendManager() = default;

public:
	static auto GetInstance() -> SendManager&
	{
		static SendManager instance;
		return instance;
	}

	auto Push(SendBufferRef buffer) -> void;
	auto Execute() -> void;
private:
	Concurrency::concurrent_queue<SendBufferRef> _send_list{};
};