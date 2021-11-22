#pragma once

/*--------------
	NetAddress
---------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(std::wstring ip, uint16 port);

	auto GetSockAddr() -> SOCKADDR_IN& ;
	auto GetIpAddress() ->std::wstring;
	auto GetPort() const -> uint16 ;

public:
	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		_sock_addr = {};
};

