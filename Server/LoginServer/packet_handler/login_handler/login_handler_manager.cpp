#include "pch.h"
#include "login_handler_manager.h"

#include "login_handler.h"

LoginHandlerManager::LoginHandlerManager() :
	_handle(std::make_shared<LoginHandler>())
{
}

void LoginHandlerManager::LoginRequest(const PacketSessionRef& session, const Protocol::LoginClientLogin& pkt) const
{
	_handle->DoAsync(&LoginHandler::LoginRequest, session, pkt);
}

void LoginHandlerManager::CharacterListRequest(const PacketSessionRef& session,
	const Protocol::LoginClientCharacterList& pkt) const
{
	_handle->DoAsync(&LoginHandler::CharacterListRequest, session, pkt);
}

void LoginHandlerManager::CreateCharacterRequest(const PacketSessionRef& session,
                                                 const Protocol::LoginClientCreateCharacter& pkt) const
{
	_handle->DoAsync(&LoginHandler::CreateCharacter, session, pkt);
}

void LoginHandlerManager::CharacterSelectRequest(const PacketSessionRef& session,
	const Protocol::LoginClientCharacterSelect& pkt) const
{
	_handle->DoAsync(&LoginHandler::SelectCharacter, session, pkt);
}
