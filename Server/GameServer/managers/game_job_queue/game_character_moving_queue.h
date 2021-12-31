#pragma once
#include "protocol/game_protocol.pb.h"

class GameCharacterMovingQueue : public JobQueue
{
public:
	static auto GetInstance() ->std::shared_ptr<GameCharacterMovingQueue>&
	{
		static auto instance = std::make_shared<GameCharacterMovingQueue>();
		return instance;
	}

public:
	auto MovePlayer(PacketSessionRef session, Protocol::GameClientMovePlayer pkt)->void;

};

