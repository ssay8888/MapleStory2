#include "pch.h"
#include "client_packet_handler.h"

#include "player/player.h"

PacketHandlerFunc ClientPacketHandler::_packet_handler[UINT16_MAX]{};

bool HandleInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)
{
	return true;
}

bool HandleClientLogin(PacketSessionRef& session, Protocol::ClientLogin& pkt)
{
	return true;
}
