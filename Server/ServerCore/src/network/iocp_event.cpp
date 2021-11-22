#include "pch.h"
#include "iocp_event.h"

IocpEvent::IocpEvent(const EventType type) :
	event_type(type)
{
	Init();
}

auto IocpEvent::Init() -> void
{
	OVERLAPPED::hEvent = nullptr;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}

ConnectEvent::ConnectEvent(): IocpEvent(EventType::kConnect)
{
	
}

DisconnectEvent::DisconnectEvent(): IocpEvent(EventType::kDisconnect)
{
	
}

AcceptEvent::AcceptEvent(): IocpEvent(EventType::kAccept)
{
	
}

RecvEvent::RecvEvent(): IocpEvent(EventType::kRecv)
{
	
}

SendEvent::SendEvent(): IocpEvent(EventType::kSend)
{
	
}
