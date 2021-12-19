#pragma once
#include "src/network/session.h"

class Player;

class GameSession : public PacketSession
{
public:
	GameSession();
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

public:
	virtual auto OnConnected() -> void override;
	virtual auto OnDisconnected() -> void override;
	virtual auto OnRecvPacket(BYTE* buffer, int32_t len) -> void override;
	virtual auto OnSend(int32_t len) -> void override;

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
};
