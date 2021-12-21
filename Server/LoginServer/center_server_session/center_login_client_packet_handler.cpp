#include "pch.h"
#include "center_login_client_packet_handler.h"

#include "center_login_session.h"
#include "packet_handler/center_handler/center_handler_manager.h"

PacketHandlerFunc CenterLoginClientPacketHandler::_packet_handler[UINT16_MAX]{};

auto CenterLoginClientPacketHandler::HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return true;
}

auto CenterLoginClientPacketHandler::HandleCenterLoginClientInit(PacketSessionRef& session,
	Protocol::CenterLoginClientInit& pkt) -> bool
{
	CenterHandlerManager::GetInstance().SocketConnectInit(session, pkt);
	return true;
}

auto CenterLoginClientPacketHandler::HandleCenterLoginClientCreateAuth(PacketSessionRef& session,
	Protocol::CenterLoginClientCreateAuth& pkt) -> bool
{
	return true;
}

auto CenterLoginClientPacketHandler::HandleCenterLoginClientCheckAuth(PacketSessionRef& session,
	Protocol::CenterLoginClientCheckAuth& pkt) -> bool
{
	return true;
}
