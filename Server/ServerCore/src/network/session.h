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
						/* 외부에서 사용 */
	auto Send(SendBufferRef sendBuffer) -> void;
	auto Connect() -> bool;
	auto Disconnect(const WCHAR* cause) -> void;

	auto GetService() -> std::shared_ptr<Service> ;
	auto SetService(std::shared_ptr<Service> service) -> void ;

public:
						/* 정보 관련 */
	auto SetNetAddress(NetAddress address) -> void ;
	auto GetAddress() const -> NetAddress ;
	auto GetSocket() const -> SOCKET ;
	auto IsConnected() const -> bool ;
	auto GetSessionRef() -> SessionRef ;
	auto GetRecvBuffer() -> RecvBuffer&;

private:
						/* 인터페이스 구현 */
	virtual auto GetHandle() -> HANDLE	 override;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32_t numOfBytes = 0) -> void override;

private:
						/* 전송 관련 */
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
						/* 컨텐츠 코드에서 재정의 */
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

							/* 수신 관련 */
	RecvBuffer				_recv_buffer;

							/* 송신 관련 */
	Queue<SendBufferRef>	_send_queue;
	Atomic<bool>			_send_registered = false;

private:
						/* IocpEvent 재사용 */
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