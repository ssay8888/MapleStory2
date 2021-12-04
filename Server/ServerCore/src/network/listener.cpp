#include "pch.h"
#include "listener.h"
#include "socket_utils.h"
#include "iocp_event.h"
#include "session.h"
#include "service.h"

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		xdelete(acceptEvent);
	}
}

auto Listener::StartAccept(ServerServiceRef service) -> bool
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32_t acceptCount = _service->GetMaxSessionCount();
	for (int32_t i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

auto Listener::CloseSocket() -> void
{
	SocketUtils::Close(_socket);
}

auto Listener::GetHandle() -> HANDLE
{
	return reinterpret_cast<HANDLE>(_socket);
}

auto Listener::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes) -> void
{
	ASSERT_CRASH(iocpEvent->event_type == EventType::kAccept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

auto Listener::RegisterAccept(AcceptEvent* acceptEvent) -> void
{
	SessionRef session = _service->CreateSession();

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recv_buffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32_t errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}

auto Listener::ProcessAccept(AcceptEvent* acceptEvent) -> void
{
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32_t sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}