#include "pch.h"
#include "login_handler_manager.h"

#include "login_handler.h"

LoginHandlerManager::LoginHandlerManager() :
	_handle(std::make_shared<LoginHandler>())
{
}

LoginHandlerManager::~LoginHandlerManager()
{
}

void LoginHandlerManager::LoginRequest(const PacketSessionRef& session, const Protocol::ClientLogin& pkt) const
{
	_handle->DoAsync(&LoginHandler::LoginRequest, session, pkt);
}

void LoginHandlerManager::CharacterListRequest(const PacketSessionRef& session,
	const Protocol::ClientCharacterList& pkt) const
{
	_handle->DoAsync(&LoginHandler::CharacterListRequest, session, pkt);
}

void LoginHandlerManager::CreateCharacterRequest(const PacketSessionRef& session,
                                                 const Protocol::ClientCreateCharacter& pkt) const
{
	_handle->DoAsync(&LoginHandler::CreateCharacter, session, pkt);
}
