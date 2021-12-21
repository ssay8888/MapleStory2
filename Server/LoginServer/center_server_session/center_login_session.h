#pragma once
#include "src/network/session.h"

class Login;
class Player;

class CenterLoginSession : public PacketSession
{
public:
	CenterLoginSession();
	~CenterLoginSession()
	{
		std::cout << "~LoginSession" << std::endl;
	}

public:
	auto OnConnected() -> void override;
	auto OnDisconnected() -> void override;
	auto OnRecvPacket(BYTE* buffer, int32_t len) -> void override;
	auto OnSend(int32_t len) -> void override;

public:
	auto GetSessionId() const->int64_t;

	auto InitBool()->void;

private:
	bool		_init;
	int64_t		_session_id = -1;
};
