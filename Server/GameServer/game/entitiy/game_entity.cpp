#include "game_server_pch.h"
#include "game_entity.h"


GameEntity::GameEntity() :
	_object_id(default_object_id++)
{

}

auto GameEntity::GetObjectId() const -> int64_t
{
	return _object_id;
}
