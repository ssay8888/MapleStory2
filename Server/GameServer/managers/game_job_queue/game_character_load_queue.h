#pragma once
#include "protocol/game_protocol.pb.h"

class GameCharacterLoadQueue : public JobQueue
{
public:
	static auto GetInstance() ->std::shared_ptr<GameCharacterLoadQueue>&
	{
		static auto instance = MakeShared<GameCharacterLoadQueue>();
		return instance;
	}

public:
	auto GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt)->void;

private:
	auto LoadCharacterInfo()->Protocol::GameServerLoadCharacter;


};

