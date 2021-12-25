#pragma once
#include "src/network/session.h"

class Character;

class GameSession final : public PacketSession
{
public:
	GameSession();
	~GameSession()
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

	auto SetPlayer(std::shared_ptr<Character> character)->void;
	auto GetPlayer() const->std::shared_ptr<Character>;

private:
	int64_t										_session_id = -1;
	int64_t										_account_id = -1;
	std::shared_ptr<Character>					_character;
};
