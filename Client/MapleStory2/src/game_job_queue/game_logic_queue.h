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
	auto AddUser(PacketSessionRef session, Protocol::GameServerRespawnPlayer pkt)->void;
	auto MovePlayer(PacketSessionRef session, Protocol::GameServerMovePlayer pkt)->void;
	auto RespawnMonster(PacketSessionRef session, Protocol::GameServerRespawnMonster pkt)->void;
	auto MoveMonster(PacketSessionRef session, Protocol::GameServerMoveMonster pkt)->void;
	auto UpdateStat(PacketSessionRef session, Protocol::GameServerUpdateStat pkt)->void;
	auto UpdateMonsterStat(PacketSessionRef session, Protocol::GameServerMonsterStatUpdate pkt)->void;
	auto KillMonster(PacketSessionRef session, Protocol::GameServerKillMonster pkt)->void;
	auto TakeDamage(PacketSessionRef session, Protocol::GameServerTakeDamage pkt)->void;
	auto DressChange(PacketSessionRef session, Protocol::GameServerDressChange pkt)->void;
	auto StatupChange(PacketSessionRef session, Protocol::GameServerStatUp pkt)->void;
	auto ItemQuantityUpdate(PacketSessionRef session, Protocol::GameServerItemQuantityUpdate pkt)->void;
	auto ResurrectionPlayer(PacketSessionRef session, Protocol::GameServerResurrection pkt)->void;


private:
	Protocol::GameServerLoadCharacter _character_info;
};

