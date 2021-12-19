#pragma once
#include "protocol/protocol.pb.h"

class GameSession;

class LoginHandler : public JobQueue
{
public:
	LoginHandler() = default;
	virtual ~LoginHandler() = default;

public:
#pragma region LoginRequest
	void LoginRequest(PacketSessionRef session, Protocol::ClientLogin pkt);
#pragma endregion


#pragma region CharacterListRequest
	void CharacterListRequest(PacketSessionRef session, Protocol::ClientCharacterList pkt);
	void CharacterListSend(std::shared_ptr<GameSession> session) const;
#pragma endregion


#pragma region CreateCharacter
	void CreateCharacter(PacketSessionRef session, Protocol::ClientCreateCharacter pkt);
#pragma endregion
}; 

