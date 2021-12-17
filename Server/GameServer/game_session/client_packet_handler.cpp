#include "pch.h"
#include "client_packet_handler.h"

#include "packet_handler/login_handler/login_handler_manager.h"

PacketHandlerFunc ClientPacketHandler::_packet_handler[UINT16_MAX]{};

bool HandleInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)
{
	return true;
}

bool HandleClientLogin(PacketSessionRef& session, Protocol::ClientLogin& pkt)
{
	LoginHandlerManager::GetInstance().LoginRequest(session, pkt);
	return true;
}

bool HandleClientCreateCharacter(PacketSessionRef& session, Protocol::ClientCreateCharacter& pkt)
{
	return false;
}
