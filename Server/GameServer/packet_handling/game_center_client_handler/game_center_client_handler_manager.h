#pragma once
#include "protocol/center_login_protocol.pb.h"
class GameCenterClientHandler;

class GameCenterClientHandlerManager
{
private:
	GameCenterClientHandlerManager();
	~GameCenterClientHandlerManager() = default;

public:
	static auto GetInstance()->GameCenterClientHandlerManager&
	{
		static GameCenterClientHandlerManager instance;
		return instance;
	}

	void CenterLoginServerCreateAuth(PacketSessionRef& session, Protocol::CenterLoginServerCreateAuth& pkt) const;
private:
	std::shared_ptr<GameCenterClientHandler> _handle = nullptr;
};

