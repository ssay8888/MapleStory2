#include "game_server_pch.h"
#include "game_character_load_queue.h"

#include <src/database/db_bind.h>
#include "managers/auth_manager/game_auth_manager.h"
#include "src/database/db_connection_pool.h"

auto GameCharacterLoadQueue::GameClientLoginResponse(PacketSessionRef session, Protocol::GameClientLogin pkt) -> void
{
	//ó�� ������ �ε��� ������ ĳ������ ������ �޾ƿ��°�.
	auto authManager = GameAuthManager::GetInstance();
	auto authInfo = authManager.FindAuth(pkt.auth());

	if (authInfo)
	{
		if (auto con = DBConnectionPool::GetInstance().Pop())
		{
			DBBind<1, 9> bind(*con, L"{CALL dbo.spLoadCharacter(?, ?, ?, ?, ?)}");




			DBConnectionPool::GetInstance().Push(con);
		}

	}
}

auto GameCharacterLoadQueue::LoadCharacterInfo() -> Protocol::GameServerLoadCharacter
{
	Protocol::GameServerLoadCharacter sendPkt;

	return sendPkt;
}
