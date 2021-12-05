#pragma once
#include "protocol/enum.pb.h"

class LoginJobQueue : public JobQueue
{
public:
	LoginJobQueue() = default;
	virtual ~LoginJobQueue() = default;

	auto LoginAttempt(PacketSessionRef session, Protocol::ServerLogin pkt)-> Protocol::kLoginMessage;
	
};

