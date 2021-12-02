#pragma once

/*--------------
	NetAddress
---------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16_t port);

	auto GetSockAddr() -> SOCKADDR_IN& ;
	auto GetIpAddress() ->std::wstring;
	auto GetPort() const -> uint16_t ;

public:
	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		_sock_addr = {};
};

