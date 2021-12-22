#include "c_pch.h"
#include "login_server_packet_handler.h"

#include "src/scene/logo/login_manager/login_manager.h"

PacketHandlerFunc LoginServerPacketHandler::_packet_handler[UINT16_MAX]{};

auto LoginServerPacketHandler::HandleLoginInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto LoginServerPacketHandler::HandleLoginServerLogin(const PacketSessionRef& session,
                                                      const Protocol::LoginServerLogin& pkt) -> bool
{
	LoginManager::GetInstance().LoginResponse(session, pkt);
	return true;
}

auto LoginServerPacketHandler::HandleLoginServerCharacterList(const PacketSessionRef& session,
                                                              const Protocol::LoginServerCharacterList& pkt) -> bool
{
	LoginManager::GetInstance().CharacterListResponse(session, pkt);
	return true;
}

auto LoginServerPacketHandler::HandleLoginServerCreateCharacter(const PacketSessionRef& session,
                                                                const Protocol::LoginServerCreateCharacter& pkt) -> bool
{
	LoginManager::GetInstance().CreateCharacterResponse(session, pkt);
	return true;
}

auto LoginServerPacketHandler::HandleLoginServerCharacterSelect(const PacketSessionRef& session,
                                                                const Protocol::LoginServerCharacterSelect& pkt) -> bool
{
	LoginManager::GetInstance().CharacterSelectResponse(session, pkt);
	return true;
}
