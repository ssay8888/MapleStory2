#pragma once
#include "protocol/center_login_protocol.pb.h"

class CenterHandler : public JobQueue
{
public:
	CenterHandler() = default;
	virtual ~CenterHandler() = default;

	void SocketConnectInit(PacketSessionRef session, Protocol::CenterLoginClientInit pkt);
	void CenterLoginClientCheckAuth(PacketSessionRef session, Protocol::CenterLoginClientCheckAuth pkt);
};

