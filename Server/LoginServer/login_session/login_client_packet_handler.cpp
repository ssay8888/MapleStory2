#include "pch.h"
#include "login_client_packet_handler.h"

#include "packet_handler/login_handler/login_handler_manager.h"

PacketHandlerFunc LoginClientPacketHandler::_packet_handler[UINT16_MAX]{};

bool LoginClientPacketHandler::HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)
{
	return true;
}


bool LoginClientPacketHandler::HandleLoginClientLogin(const PacketSessionRef& session, 
	const Protocol::LoginClientLogin& pkt)
{
	LoginHandlerManager::GetInstance().LoginRequest(session, pkt);
	return true;
}

bool LoginClientPacketHandler::HandleLoginClientCharacterList(const PacketSessionRef& session, 
	const Protocol::LoginClientCharacterList& pkt)
{
	LoginHandlerManager::GetInstance().CharacterListRequest(session, pkt);
	return true;
}

bool LoginClientPacketHandler::HandleLoginClientCreateCharacter(const PacketSessionRef& session, 
	const Protocol::LoginClientCreateCharacter& pkt)
{
	LoginHandlerManager::GetInstance().CreateCharacterRequest(session, pkt);
	return true;
}

auto LoginClientPacketHandler::HandleLoginClientCharacterSelect(const PacketSessionRef& session,
	const Protocol::LoginClientCharacterSelect& pkt) -> bool
{
	LoginHandlerManager::GetInstance().CharacterSelectRequest(session, pkt);
	return true;
}