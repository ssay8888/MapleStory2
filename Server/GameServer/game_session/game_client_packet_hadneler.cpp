#include "game_server_pch.h"
#include "game_client_packet_handler.h"

PacketHandlerFunc GameClientPacketHandler::_packet_handler[UINT16_MAX]{};

auto GameClientPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameClientPacketHandler::HandleGameClientLogin(PacketSessionRef& session, Protocol::GameClientLogin& pkt) -> bool
{
	return false;
}
