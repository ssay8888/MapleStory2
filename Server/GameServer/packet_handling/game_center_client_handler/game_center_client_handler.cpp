#include "game_server_pch.h"
#include "game_center_client_handler.h"

#include "center_client_session/center_login_server_packet_handler.h"
#include "managers/auth_manager/game_auth_manager.h"

void GameCenterClientHandler::CenterLoginServerCreateAuth(PacketSessionRef session,
	Protocol::CenterLoginServerCreateAuth pkt)
{
	GameAuthManager::GetInstance().AddAuth(pkt);

	Protocol::CenterLoginClientCheckAuth sendPkt;
	sendPkt.set_accountid(pkt.accountid());
	sendPkt.set_auth(pkt.auth());
	sendPkt.set_characterid(pkt.characterid());

	session->Send(CenterLoginServerPacketHandler::MakeSendBuffer(sendPkt));
}
