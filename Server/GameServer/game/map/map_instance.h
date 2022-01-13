#pragma once
#include "map_manager.h"
#include "game_session/game_client_packet_handler.h"
#include "src/utility/components/collider/collider.h"
class GameEntity;
class MapXblock;
class GameMonster;
class MapObject;
class GameCharacter;

class MapInstance : public std::enable_shared_from_this<MapInstance>
{
public:
	explicit MapInstance(int32_t mapId);
	~MapInstance() = default;

public:
	/* DoAsync */
	auto AddCharacter(std::shared_ptr<GameSession> session)->bool;
	auto RemoveCharacter(int64_t characterId)->bool;
	auto FindCharacter(int64_t characterId)->std::shared_ptr<GameSession>;
	auto FindCharacter(_float3 pos, float distance)->std::shared_ptr<GameCharacter>;

	template<typename T>
	auto BroadCastMessage(T sendPkt, std::shared_ptr<GameSession> gameSession)->void;

	auto BroadCastAddCharacter(std::shared_ptr<GameSession> session)->void;
	auto RespawnAllMonster(std::shared_ptr<GameSession> session)->void;

public:
	auto Respawn()->void;

public:
	auto GetMapId()const->int32_t;

	auto SetSpawnPoint(std::vector<_float3> sp)->void;
	auto GetSpawnPoint(int32_t index)->const _float3* const;

	auto SetRegionPoint(std::vector<std::shared_ptr<SpawnPoint>> sp)->void;
	auto GetRegionPoint(int32_t index)->std::shared_ptr<SpawnPoint>;

	auto AddObjects(std::vector<MapManager::MapEntity> objects)->void;
	auto GetObjects()const->std::vector<std::shared_ptr<MapXblock>>;

	auto GetMonsters() -> std::map<std::shared_ptr<SpawnPoint>, std::shared_ptr<GameMonster>>;
	auto FindMonster(int64_t objectId)const->std::shared_ptr<GameMonster>;
	auto RemoveMonster(int64_t objectId) -> void;
	auto FindRangeCellObject(
		const std::shared_ptr<Collider>& targetCollider) -> std::vector<std::shared_ptr<MapXblock>>;


private:
	const int32_t															_map_id;

	std::vector<_float3>													_spawn_points;
	std::vector<std::shared_ptr<SpawnPoint>>								_region_points;
	std::vector<std::shared_ptr<MapXblock>>									_objects;
	std::map<int64_t, std::shared_ptr<GameSession>>							_characters;
	std::map<std::shared_ptr<SpawnPoint>, std::shared_ptr<GameMonster>>		_spawn_point_monsters;
	std::map<int64_t, std::shared_ptr<GameEntity>>							_entities;
};

template <typename T>
auto MapInstance::BroadCastMessage(T sendPkt, std::shared_ptr<GameSession> gameSession)->void
{
	for (auto& [key, session] : _characters)
	{
		if (gameSession != session)
		{
			session->Send(GameClientPacketHandler::MakeSendBuffer(sendPkt));
		}
	}
}
