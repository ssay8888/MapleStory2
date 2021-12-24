#include "game_server_pch.h"
#include "game_character.h"

#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"

GameCharacter::GameCharacter(const int64_t characterId):
	_character_id(characterId)
{
}

auto GameCharacter::NativeConstruct() -> HRESULT
{
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 9> bind(*con, L"{CALL dbo.spLoadCharacter(?)}");
		

		DBConnectionPool::GetInstance().Push(con);
	}
	return S_OK;
}
