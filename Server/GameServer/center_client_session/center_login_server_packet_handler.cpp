#include "game_server_pch.h"
#include "center_login_server_packet_handler.h"

#include "managers/auth_manager/game_auth_manager.h"
#include "packet_handling/game_center_client_handler/game_center_client_handler_manager.h"

PacketHandlerFunc CenterLoginServerPacketHandler::_packet_handler[UINT16_MAX]{};
auto CenterLoginServerPacketHandler::HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return true;
}

auto CenterLoginServerPacketHandler::HandleCenterLoginServerInit(PacketSessionRef& session,
	Protocol::CenterLoginServerInit& pkt) -> bool
{
	return true;
}

auto CenterLoginServerPacketHandler::HandleCenterLoginServerCreateAuth(PacketSessionRef& session,
	Protocol::CenterLoginServerCreateAuth& pkt) -> bool
{
	GameCenterClientHandlerManager::GetInstance().CenterLoginServerCreateAuth(session, pkt);
	return true;
}

auto CenterLoginServerPacketHandler::HandleCenterLoginServerCheckAuth(PacketSessionRef& session,
	Protocol::CenterLoginServerCheckAuth& pkt) -> bool
{
	return true;
}
