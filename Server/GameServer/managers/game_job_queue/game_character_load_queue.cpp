#include "game_server_pch.h"
#include "game_character_load_queue.h"

#include "managers/auth_manager/game_auth_manager.h"

auto GameCharacterLoadQueue::GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt) -> void
{
	//ó�� ������ �ε��� ������ ĳ������ ������ �޾ƿ��°�.
	auto authManager = GameAuthManager::GetInstance();
	auto authInfo = authManager.FindAuth(pkt.auth());


}

auto GameCharacterLoadQueue::LoadCharacterInfo() -> Protocol::GameServerLoadCharacter
{
	Protocol::GameServerLoadCharacter sendPkt;

	return sendPkt;
}
