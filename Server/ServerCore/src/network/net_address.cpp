#include "pch.h"
#include "net_address.h"

/*--------------
	NetAddress
---------------*/

NetAddress::NetAddress(const SOCKADDR_IN sockAddr) :
	_sock_addr(sockAddr)
{
}

NetAddress::NetAddress(const std::wstring ip, const uint16_t port)
{
	memset(&_sock_addr, 0, sizeof(_sock_addr));
	_sock_addr.sin_family = AF_INET;
	_sock_addr.sin_addr = Ip2Address(ip.c_str());
	_sock_addr.sin_port = htons(port);
}

auto NetAddress::GetSockAddr() -> SOCKADDR_IN&
{
	return _sock_addr;
}

auto NetAddress::GetIpAddress() -> std::wstring
{
	WCHAR buffer[100];
	InetNtopW(AF_INET, &_sock_addr.sin_addr, buffer, len32(buffer));
	return std::wstring(buffer);
}

auto NetAddress::GetPort() const -> uint16_t
{
	return ntohs(_sock_addr.sin_port);
}

auto NetAddress::Ip2Address(const WCHAR* ip) -> IN_ADDR
{
	IN_ADDR address;
	InetPtonW(AF_INET, ip, &address);
	return address;
}
