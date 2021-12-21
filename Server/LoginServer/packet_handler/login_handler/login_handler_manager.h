#pragma once

class LoginHandler;

class LoginHandlerManager
{
private:
	LoginHandlerManager();
	~LoginHandlerManager() = default;

public:
	static auto GetInstance()->LoginHandlerManager&
	{
		static LoginHandlerManager instance;
		return instance;
	}

	void LoginRequest(const PacketSessionRef& session, const Protocol::LoginClientLogin& pkt) const;
	void CharacterListRequest(const PacketSessionRef& session, const Protocol::LoginClientCharacterList& pkt) const;
	void CreateCharacterRequest(const PacketSessionRef& session, const Protocol::LoginClientCreateCharacter& pkt) const;
	void CharacterSelectRequest(const PacketSessionRef& session, const Protocol::LoginClientCharacterSelect& pkt) const;
	
private:
	std::shared_ptr<LoginHandler> _handle = nullptr;
};

