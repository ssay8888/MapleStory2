#pragma once
#include "protocol/enum.pb.h"

class LoginJobQueue : public JobQueue
{
public:
	LoginJobQueue() = default;
	virtual ~LoginJobQueue() = default;

	auto LoginAttempt(PacketSessionRef session, Protocol::ServerLogin pkt)-> Protocol::kLoginMessage;
	auto LoadCharacterList(PacketSessionRef session, Protocol::ServerCharacterList pkt)-> void;
	auto CreateCharacter(PacketSessionRef session, Protocol::ServerCreateCharacter pkt)-> Protocol::kCreateMessage;
	
};

