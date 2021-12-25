#include "game_server_pch.h"
#include "base_info.h"

BaseInfo::BaseInfo(const int64_t characterId) :
	_character_id(characterId)
{
}

auto BaseInfo::GetCharacterId() const -> int64_t
{
	return _character_id;
}
