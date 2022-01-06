#include "game_server_pch.h"
#include "spawn_point.h"

auto SpawnPoint::SetPosition(const _float3 position) -> void
{
	_position = position;
}

auto SpawnPoint::GetPosition() const -> _float3
{
	return _position;
}

auto SpawnPoint::SetSpawnNpcId(const int32_t id) -> void
{
	_spawn_npc_id = id;
}

auto SpawnPoint::GetSpawnNpcId() const -> int32_t
{
	return _spawn_npc_id;
}
