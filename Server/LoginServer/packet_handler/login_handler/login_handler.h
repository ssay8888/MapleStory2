#pragma once

class LoginSession;

class LoginHandler : public JobQueue
{
public:
	LoginHandler() = default;
	virtual ~LoginHandler() = default;

public:
#pragma region LoginRequest
	enum class kSqlLoginResult
	{
		kIdError = -1,
		kAlreadyConnected = -2,
		kPasswordError = -3,
		kLoginSuccess = 1
	};
	void LoginRequest(PacketSessionRef session, Protocol::LoginClientLogin pkt);
#pragma endregion


#pragma region CharacterListRequest
	void CharacterListRequest(PacketSessionRef session, Protocol::LoginClientCharacterList pkt);
	void CharacterListSend(std::shared_ptr<LoginSession> session) const;
#pragma endregion


#pragma region CreateCharacter
	enum class kSqlCreateCharacterResult
	{
		kIdDuplication = -1,
		kCreateFailed = -2,
		kCreateSuccess = 1
	};
	void CreateCharacter(PacketSessionRef session, Protocol::LoginClientCreateCharacter pkt);
	std::wstring ItemListToXml(Protocol::LoginClientCreateCharacter pkt);
#pragma endregion

#pragma region SelectCharacter
	void SelectCharacter(PacketSessionRef session, Protocol::LoginClientCharacterSelect pkt);
#pragma endregion
}; 

