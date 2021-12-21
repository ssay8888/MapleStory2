#pragma once
#include "src/network/session.h"

class Login;
class Player;

class LoginSession : public PacketSession
{
public:
	LoginSession();
	~LoginSession()
	{
		std::cout << "~LoginSession" << std::endl;
	}

public:
	auto OnConnected() -> void override;
	auto OnDisconnected() -> void override;
	auto OnRecvPacket(BYTE* buffer, int32_t len) -> void override;
	auto OnSend(int32_t len) -> void override;

public:
	auto GetSessionId() const->int64_t;
	auto GetAccountId() const->int64_t;
	auto SetAccountId(int64_t id) ->void;

	auto PushPlayer(std::shared_ptr<Player> player)->void;
	auto GetPlayerList() const ->std::vector<std::shared_ptr<Player>>;

private:
	int64_t										_session_id = -1;
	int64_t										_account_id = -1;
	std::vector<std::shared_ptr<Player>>		_players;
	std::shared_ptr<Login>						_login;
};
