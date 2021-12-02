#pragma once
#include "iocp_core.h"
#include "iocp_event.h"
#include "net_address.h"
#include "recv_buffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	Session();
	virtual ~Session();

public:
						/* �ܺο��� ��� */
	auto Send(SendBufferRef sendBuffer) -> void;
	auto Connect() -> bool;
	auto Disconnect(const WCHAR* cause) -> void;

	auto GetService() -> std::shared_ptr<Service> ;
	auto SetService(std::shared_ptr<Service> service) -> void ;

public:
						/* ���� ���� */
	auto SetNetAddress(NetAddress address) -> void ;
	auto GetAddress() const -> NetAddress ;
	auto GetSocket() const -> SOCKET ;
	auto IsConnected() const -> bool ;
	auto GetSessionRef() -> SessionRef ;
	auto GetRecvBuffer() -> RecvBuffer&;

private:
						/* �������̽� ���� */
	virtual auto GetHandle() -> HANDLE	 override;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32_t numOfBytes = 0) -> void override;

private:
						/* ���� ���� */
	auto RegisterConnect() -> bool;
	auto RegisterDisconnect()-> bool;
	auto RegisterRecv() -> void;
	auto RegisterSend() -> void;
		 
	auto ProcessConnect() -> void;
	auto ProcessDisconnect() -> void;
	auto ProcessRecv(int32_t numOfBytes) -> void;
	auto ProcessSend(int32_t numOfBytes) -> void;
		 
	auto HandleError(int32_t errorCode) -> void;

protected:
						/* ������ �ڵ忡�� ������ */
	virtual auto OnConnected() -> void { }
	virtual auto OnRecv(BYTE* buffer, int32_t len) -> int32_t ;
	virtual auto OnSend(int32_t len) -> void { }
	virtual auto OnDisconnected() -> void { }

private:
	std::weak_ptr<Service>	_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_net_address = {};
	Atomic<bool>			_connected = false;

protected:
	USE_LOCK;

							/* ���� ���� */
	RecvBuffer				_recv_buffer;

							/* �۽� ���� */
	Queue<SendBufferRef>	_send_queue;
	Atomic<bool>			_send_registered = false;

private:
						/* IocpEvent ���� */
	ConnectEvent		_connect_event;
	DisconnectEvent		_disconnect_event;
	RecvEvent			_recv_event;
	SendEvent			_send_event;
};

struct PacketHeader
{
	uint16_t size;
	uint16_t id;
};

class PacketSession : public Session
{
public:
	PacketSession() = default;
	virtual ~PacketSession() = default;

	auto GetPacketSessionRef() -> PacketSessionRef ;

protected:
	virtual auto OnRecv(BYTE* buffer, int32_t len) -> int32_t sealed;
	virtual auto OnRecvPacket(BYTE* buffer, int32_t len) ->void abstract;
};