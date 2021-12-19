#pragma once
class LoginJobQueue;

class LoginManager
{
private:
	LoginManager();
	~LoginManager() = default;
public:
	static auto GetInstance() -> LoginManager&
	{
		static LoginManager instance;
		return instance;
	}

	void LoginResponse(PacketSessionRef& session, Protocol::ServerLogin& pkt);
	void CharacterListResponse(PacketSessionRef& session, Protocol::ServerCharacterList& pkt);
	void CreateCharacterResponse(PacketSessionRef& session, Protocol::ServerCreateCharacter& pkt);

private:
	std::shared_ptr<LoginJobQueue> _login_queue;
};