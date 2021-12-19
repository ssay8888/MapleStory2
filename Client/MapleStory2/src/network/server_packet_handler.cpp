#include "c_pch.h"
#include "server_packet_handler.h"

#include "src/scene/logo/login_manager/login_manager.h"

PacketHandlerFunc ServerPacketHandler::_packet_handler[UINT16_MAX]{};

bool HandleInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len)
{
	return true;
}

bool HandleServerLogin(PacketSessionRef& session, Protocol::ServerLogin& pkt)
{
	LoginManager::GetInstance().LoginResponse(session, pkt);
	return true;
}

bool HandleServerCharacterList(PacketSessionRef& session, Protocol::ServerCharacterList& pkt)
{
	LoginManager::GetInstance().CharacterListResponse(session, pkt);
	return true;
}

bool HandleServerCreateCharacter(PacketSessionRef& session, Protocol::ServerCreateCharacter& pkt)
{
	LoginManager::GetInstance().CreateCharacterResponse(session, pkt);
	return true;
}
