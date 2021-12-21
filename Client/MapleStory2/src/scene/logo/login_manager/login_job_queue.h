#pragma once

class LoginJobQueue : public JobQueue
{
public:
	LoginJobQueue() = default;
	virtual ~LoginJobQueue() = default;

	auto LoginAttempt(PacketSessionRef session, Protocol::LoginServerLogin pkt)-> Protocol::kLoginMessage;
	auto LoadCharacterList(PacketSessionRef session, Protocol::LoginServerCharacterList pkt)-> void;
	auto CreateCharacter(PacketSessionRef session, Protocol::LoginServerCreateCharacter pkt)-> Protocol::kCreateMessage;
	
};

