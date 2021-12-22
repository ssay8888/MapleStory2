#include "game_server_pch.h"
#include "game_character_load_queue.h"

#include "managers/auth_manager/game_auth_manager.h"

auto GameCharacterLoadQueue::GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt) -> void
{
	//처음 유저가 로딩이 끝나고 캐릭터의 정보를 받아오는곳.
	auto authManager = GameAuthManager::GetInstance();
	auto authInfo = authManager.FindAuth(pkt.auth());


}

auto GameCharacterLoadQueue::LoadCharacterInfo() -> Protocol::GameServerLoadCharacter
{
	Protocol::GameServerLoadCharacter sendPkt;

	return sendPkt;
}
