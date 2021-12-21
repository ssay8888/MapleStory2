#include "game_server_pch.h"
#include "game_center_client_handler_manager.h"

#include "game_center_client_handler.h"

GameCenterClientHandlerManager::GameCenterClientHandlerManager() :
	_handle(std::make_shared<GameCenterClientHandler>())
{
}

void GameCenterClientHandlerManager::CenterLoginServerCreateAuth(PacketSessionRef& session,
	Protocol::CenterLoginServerCreateAuth& pkt) const
{
	_handle->DoAsync(&GameCenterClientHandler::CenterLoginServerCreateAuth, session, pkt);
}
