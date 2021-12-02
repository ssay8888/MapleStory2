#include "pch.h"
#include "socket_utils.h"

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

auto SocketUtils::Init() -> void
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT &wsaData) == 0);
	
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
}

auto SocketUtils::Clear() -> void
{
	::WSACleanup();
}

auto SocketUtils::BindWindowsFunction(const SOCKET socket, GUID guid, LPVOID* fn) -> bool
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

auto SocketUtils::CreateSocket() -> SOCKET
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

auto SocketUtils::SetLinger(const SOCKET socket, const uint16_t onoff, const uint16_t linger)-> bool
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

auto SocketUtils::SetReuseAddress(const SOCKET socket, const bool flag)-> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

auto SocketUtils::SetRecvBufferSize(const SOCKET socket, const int32_t size)-> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

auto SocketUtils::SetSendBufferSize(const SOCKET socket, const int32_t size)-> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

auto SocketUtils::SetTcpNoDelay(const SOCKET socket, const bool flag)-> bool
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket의 특성을 ClientSocket에 그대로 적용하는 함수
auto SocketUtils::SetUpdateAcceptSocket(const SOCKET socket, const SOCKET listenSocket)-> bool
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

auto SocketUtils::Bind(const SOCKET socket, NetAddress netAddr)-> bool
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

auto SocketUtils::BindAnyAddress(const SOCKET socket, const uint16_t port)-> bool
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

auto SocketUtils::Listen(const SOCKET socket, const  int32_t backlog)-> bool
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

auto SocketUtils::Close(SOCKET& socket)  -> void
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}