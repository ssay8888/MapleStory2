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

	void LoginResponse(const PacketSessionRef& session, const Protocol::LoginServerLogin& pkt);
	void CharacterListResponse(const PacketSessionRef& session, const Protocol::LoginServerCharacterList& pkt);
	void CreateCharacterResponse(const PacketSessionRef& session, const Protocol::LoginServerCreateCharacter& pkt);
	void CharacterSelectResponse(const PacketSessionRef& session, const Protocol::LoginServerCharacterSelect& pkt);

private:
	std::shared_ptr<LoginJobQueue> _login_queue;
};