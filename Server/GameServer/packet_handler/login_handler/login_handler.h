#pragma once
#include "protocol/protocol.pb.h"

class LoginHandler : public JobQueue
{
public:
	LoginHandler() = default;
	virtual ~LoginHandler() = default;

public:
	void LoginRequest(PacketSessionRef session, Protocol::ClientLogin pkt);
};

