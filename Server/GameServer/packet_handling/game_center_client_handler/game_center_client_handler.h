#pragma once
#include "protocol/center_login_protocol.pb.h"

class GameCenterClientHandler : public JobQueue
{
public:
	GameCenterClientHandler() = default;
	virtual ~GameCenterClientHandler() = default;

	void CenterLoginServerCreateAuth(PacketSessionRef session, Protocol::CenterLoginServerCreateAuth pkt);
};

