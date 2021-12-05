#pragma once
#include "src/network/session.h"

class GameSession : public PacketSession
{
public:
	GameSession();
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

public:
	virtual auto OnConnected() -> void override;
	virtual auto OnDisconnected() -> void override;
	virtual auto OnRecvPacket(BYTE* buffer, int32_t len) -> void override;
	virtual auto OnSend(int32_t len) -> void override;

public:
	auto GetSessionId() const -> int64_t ;
private:
	int64_t						_session_id = -1;
};
