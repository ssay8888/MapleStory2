#pragma once
#include "protocol/game_protocol.pb.h"

class GameCharacter;

class GameCharacterLoadQueue : public JobQueue
{
public:
	static auto GetInstance() ->std::shared_ptr<GameCharacterLoadQueue>&
	{
		static auto instance = std::make_shared<GameCharacterLoadQueue>();
		return instance;
	}

public:
	auto GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt)->void;
	auto GameClientLoadingResponse(PacketSessionRef session, Protocol::GameClientLoading pkt)->void;
	auto SaveDbToPlayer(PacketSessionRef session)->void;

public:
	static auto SettingCharacterInfoSendPacket(Protocol::GameServerLoadCharacter& sendPkt, PacketSessionRef session, std::shared_ptr<GameCharacter> player, bool first = true) ->void;


};

