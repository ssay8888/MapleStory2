#pragma once

class Session;

enum class EventType : uint8
{
	kConnect,
	kDisconnect,
	kAccept,
	//PreRecv,
	kRecv,
	kSend
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	auto Init() -> void;

public:
	EventType		event_type;
	IocpObjectRef	owner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent();
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent();
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent();

public:
	SessionRef	session = nullptr;
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent();
};

class SendEvent : public IocpEvent
{
public:
	SendEvent();

	Vector<SendBufferRef> send_buffers;
};