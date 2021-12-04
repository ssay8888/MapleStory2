#include "pch.h"
#include "iocp_event.h"

IocpEvent::IocpEvent(const EventType type) :
	event_type(type)
{
	Init();
}

auto IocpEvent::Init() -> void
{
	hEvent = nullptr;
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
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
