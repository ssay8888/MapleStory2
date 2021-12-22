#include "pch.h"
#include "center_handler_manager.h"

#include "center_handler.h"

CenterHandlerManager::CenterHandlerManager() :
	_handle(std::make_shared<CenterHandler>())
{
}

void CenterHandlerManager::SocketConnectInit(PacketSessionRef& session, Protocol::CenterLoginClientInit& pkt) const
{
	_handle->DoAsync(&CenterHandler::SocketConnectInit, session, pkt);
}

void CenterHandlerManager::CenterLoginClientCheckAuth(PacketSessionRef& session,
	Protocol::CenterLoginClientCheckAuth& pkt) const
{
	_handle->DoAsync(&CenterHandler::CenterLoginClientCheckAuth, session , pkt);
} 
