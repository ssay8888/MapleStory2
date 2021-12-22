#include "c_pch.h"
#include "game_server_packet_handler.h"

PacketHandlerFunc GameServerPacketHandler::_packet_handler[UINT16_MAX];
auto GameServerPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameServerPacketHandler::HandleGameServerLogin(PacketSessionRef& session, Protocol::GameServerLogin& pkt) -> bool
{
	return false;
}

auto GameServerPacketHandler::HandleGameServerLoadCharacter(PacketSessionRef& session,
	Protocol::GameServerLoadCharacter& pkt) -> bool
{
	return false;
}
