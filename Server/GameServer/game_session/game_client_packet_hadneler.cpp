#include "game_server_pch.h"
#include "game_client_packet_handler.h"
#include "managers/game_job_queue/game_character_load_queue.h"

PacketHandlerFunc GameClientPacketHandler::_packet_handler[UINT16_MAX]{};

auto GameClientPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameClientPacketHandler::HandleGameClientLogin(PacketSessionRef& session, Protocol::GameClientLogin& pkt) -> bool
{
	GameCharacterLoadQueue::GetInstance()->DoAsync(&GameCharacterLoadQueue::GameClientLoginResponse, session, pkt);
	return true;
}

auto GameClientPacketHandler::HandleGameClientMovePlayer(PacketSessionRef& session,
	Protocol::GameClientMovePlayer& pkt) -> bool
{
	return true;
}
