#include "c_pch.h"
#include "game_server_packet_handler.h"

#include "src/game_job_queue/game_logic_queue.h"

PacketHandlerFunc GameServerPacketHandler::_packet_handler[UINT16_MAX];
auto GameServerPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameServerPacketHandler::HandleGameServerLogin(PacketSessionRef& session, Protocol::GameServerLogin& pkt) -> bool
{
	return true;
}

auto GameServerPacketHandler::HandleGameServerLoadCharacter(PacketSessionRef& session,
	Protocol::GameServerLoadCharacter& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::CharacterLoad, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerRespawnPlayer(PacketSessionRef& session,
	Protocol::GameServerRespawnPlayer& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::AddUser, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerMovePlayer(PacketSessionRef& session,
	Protocol::GameServerMovePlayer& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::MovePlayer, session, pkt);
	return true;
}
