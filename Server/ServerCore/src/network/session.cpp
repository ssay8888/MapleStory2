#include "pch.h"
#include "session.h"
#include "socket_utils.h"
#include "service.h"

/*--------------
	Session
---------------*/

Session::Session() :
	_recv_buffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

auto Session::Send(const SendBufferRef sendBuffer) -> void
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
	{
		WRITE_LOCK;

		_send_queue.push(sendBuffer);

		if (_send_registered.exchange(true) == false)
			registerSend = true;
	}
	
	if (registerSend)
		RegisterSend();
}

auto Session::Connect() -> bool
{
	return RegisterConnect();
}

auto Session::Disconnect(const WCHAR* cause) -> void
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	std::wcout << "Disconnect : " << cause << std::endl;

	RegisterDisconnect();
}

auto Session::GetService() -> std::shared_ptr<Service>
{
	return _service.lock();
}

auto Session::SetService(const std::shared_ptr<Service> service) -> void
{
	_service = service;
}

auto Session::SetNetAddress(NetAddress address) -> void
{
	_net_address = address;
}

auto Session::GetAddress() const -> NetAddress
{
	return _net_address;
}

auto Session::GetSocket() const -> SOCKET
{
	return _socket;
}

auto Session::IsConnected() const -> bool
{
	return _connected;
}

auto Session::GetSessionRef() -> SessionRef
{
	return std::static_pointer_cast<Session>(shared_from_this());
}

auto Session::GetRecvBuffer() -> RecvBuffer&
{
	return _recv_buffer;
}

auto Session::GetHandle() -> HANDLE
{
	return reinterpret_cast<HANDLE>(_socket);
}

auto Session::Dispatch(IocpEvent* iocpEvent, const int32_t numOfBytes) -> void
{
	switch (iocpEvent->event_type)
	{
	case EventType::kConnect:
		ProcessConnect();
		break;
	case EventType::kDisconnect:
		ProcessDisconnect();
		break;
	case EventType::kRecv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::kSend:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

auto Session::RegisterConnect() -> bool
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0) == false)
		return false;

	_connect_event.Init();
	_connect_event.owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connect_event))
	{
		int32_t errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connect_event.owner = nullptr;
			return false;
		}
	}

	return true;
}

auto Session::RegisterDisconnect() -> bool
{
	_disconnect_event.Init();
	_disconnect_event.owner = shared_from_this();

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnect_event, TF_REUSE_SOCKET, 0))
	{
		int32_t errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnect_event.owner = nullptr;
			return false;
		}
	}

	return true;
}

auto Session::RegisterRecv() -> void
{
	if (IsConnected() == false)
		return;

	_recv_event.Init();
	_recv_event.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recv_buffer.WritePos());
	wsaBuf.len = _recv_buffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recv_event, nullptr))
	{
		int32_t errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recv_event.owner = nullptr;
		}
	}
}

auto Session::RegisterSend() -> void
{
	if (IsConnected() == false)
		return;

	_send_event.Init();
	_send_event.owner = shared_from_this();
	
	{
		WRITE_LOCK;

		int32_t writeSize = 0;
		while (_send_queue.empty() == false)
		{
			SendBufferRef sendBuffer = _send_queue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : 예외 체크

			_send_queue.pop();
			_send_event.send_buffers.push_back(sendBuffer);
		}
	}
	
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_send_event.send_buffers.size());
	for (const SendBufferRef sendBuffer : _send_event.send_buffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, &_send_event, nullptr))
	{
		int32_t errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_send_event.owner = nullptr;
			_send_event.send_buffers.clear();
			_send_registered.store(false);
		}
	}
}

auto Session::ProcessConnect()-> void
{
	_connect_event.owner = nullptr;

	_connected.store(true);
	
	GetService()->AddSession(GetSessionRef());
	
	OnConnected();
	
	RegisterRecv();
}

auto Session::ProcessDisconnect()-> void
{
	_disconnect_event.owner = nullptr;

	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());
}

auto Session::ProcessRecv(const int32_t numOfBytes)-> void
{
	_recv_event.owner = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recv_buffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32_t dataSize = _recv_buffer.DataSize();
	int32_t processLen = OnRecv(_recv_buffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recv_buffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}
	
	_recv_buffer.Clean();

	RegisterRecv();
}

auto Session::ProcessSend(const int32_t numOfBytes) -> void
{
	_send_event.owner = nullptr;
	_send_event.send_buffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}
	
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_send_queue.empty())
		_send_registered.store(false);
	else
		RegisterSend();
}

auto Session::HandleError(const int32_t errorCode) -> void
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		std::cout << "Handle Error : " << errorCode << std::endl;
		break;
	}
}

auto Session::OnRecv(BYTE* buffer, const int32_t len) -> int32_t
{
	return len;
}

/*-----------------
	PacketSession
------------------*/

auto PacketSession::GetPacketSessionRef() -> PacketSessionRef
{
	return std::static_pointer_cast<PacketSession>(shared_from_this());
}

int32_t PacketSession::OnRecv(BYTE* buffer, const int32_t len)
{
	int32_t processLen = 0;

	while (true)
	{
		int32_t dataSize = len - processLen;
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		if (dataSize < header.size)
			break;
		
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}
	return processLen;
}
