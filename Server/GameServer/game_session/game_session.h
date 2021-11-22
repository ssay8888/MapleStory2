#pragma once
#include "src/network/session.h"

class Player;

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
	virtual auto OnRecvPacket(BYTE* buffer, int32 len) -> void override;
	virtual auto OnSend(int32 len) -> void override;

public:
	auto GetPlayer() const-> std::shared_ptr<Player>;
	auto GetSessionId() const -> int64 ;
private:
	int64						_session_id = -1;
	std::shared_ptr<Player>		_current_player;
	std::weak_ptr<class Room>	_room; 
};
