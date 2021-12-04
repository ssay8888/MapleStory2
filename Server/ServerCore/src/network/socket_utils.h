#pragma once
#include "net_address.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static auto Init() -> void;
	static auto Clear() -> void;

	static auto BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn) -> bool;
	static auto CreateSocket() ->SOCKET;

	static auto SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger) -> bool;
	static auto SetReuseAddress(SOCKET socket, bool flag) -> bool;
	static auto SetRecvBufferSize(SOCKET socket, int32_t size) -> bool;
	static auto SetSendBufferSize(SOCKET socket, int32_t size) -> bool;
	static auto SetTcpNoDelay(SOCKET socket, bool flag) -> bool;
	static auto SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket) -> bool;

	static auto Bind(SOCKET socket, NetAddress netAddr) -> bool;
	static auto BindAnyAddress(SOCKET socket, uint16_t port) -> bool;
	static auto Listen(SOCKET socket, int32_t backlog = SOMAXCONN) -> bool;
	static auto Close(SOCKET& socket) -> void;
};

template<typename T>
static inline bool SetSockOpt(const SOCKET socket, const int32_t level, const int32_t optName, T optVal)
{
	return SOCKET_ERROR != setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}