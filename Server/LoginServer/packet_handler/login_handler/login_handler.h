#pragma once

class LoginSession;

class LoginHandler : public JobQueue
{
public:
	LoginHandler() = default;
	virtual ~LoginHandler() = default;

public:
#pragma region LoginRequest
	void LoginRequest(PacketSessionRef session, Protocol::LoginClientLogin pkt);
#pragma endregion


#pragma region CharacterListRequest
	void CharacterListRequest(PacketSessionRef session, Protocol::LoginClientCharacterList pkt);
	void CharacterListSend(std::shared_ptr<LoginSession> session) const;
#pragma endregion


#pragma region CreateCharacter
	void CreateCharacter(PacketSessionRef session, Protocol::LoginClientCreateCharacter pkt);
#pragma endregion

#pragma region SelectCharacter
	void SelectCharacter(PacketSessionRef session, Protocol::LoginClientCharacterSelect pkt);
#pragma endregion
}; 

