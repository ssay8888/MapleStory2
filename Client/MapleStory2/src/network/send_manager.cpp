#include "c_pch.h"
#include "send_manager.h"

#include "src/network/service.h"

auto SendManager::Push(SendBufferRef buffer) -> void
{
	_send_list.push(buffer);
}

auto SendManager::Execute() -> void
{
	SendBufferRef buffer;
	while (_send_list.try_pop(buffer))
	{
		g_service->Broadcast(buffer);
	}
}
