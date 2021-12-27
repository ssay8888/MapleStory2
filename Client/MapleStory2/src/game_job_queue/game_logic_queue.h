#pragma once
#include "protocol/game_protocol.pb.h"

class GameLogicQueue : public JobQueue
{
public:
	GameLogicQueue() = default;
	~GameLogicQueue() = default;
	static auto GetInstance()->std::shared_ptr<GameLogicQueue>
	{
		static auto instance = std::make_shared<GameLogicQueue>();
		return instance;
	}

public:
	auto GetCharacterInfo() const ->Protocol::GameServerLoadCharacter;

public:
	auto CharacterLoad(PacketSessionRef session, Protocol::GameServerLoadCharacter pkt)->void;

private:
	Protocol::GameServerLoadCharacter _character_info;
};

