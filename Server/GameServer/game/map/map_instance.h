#pragma once
#include "map_manager.h"
#include "game_session/game_client_packet_handler.h"
class MapObject;
class GameCharacter;

class MapInstance : public JobQueue
{
public:
	explicit MapInstance(int32_t mapId);
	~MapInstance() = default;

public:
	/* DoAsync */
	auto AddCharacter(std::shared_ptr<GameSession> session)->bool;
	auto RemoveCharacter(int64_t characterId)->bool;
	auto FindCharacter(int64_t characterId)->std::shared_ptr<GameSession>;

	template<typename T>
	auto BroadCastMessage(T sendPkt, std::shared_ptr<GameSession> gameSession)->void;

	auto BroadCastAddCharacter(std::shared_ptr<GameSession> session)->void;

public:
	auto GetMapId()const->int32_t;

	auto SetSpawnPoint(std::vector<_float3> sp)->void;
	auto GetSpawnPoint(int32_t index)->const _float3* const;

	auto SetRegionPoint(std::vector<_float3> sp)->void;
	auto GetRegionPoint(int32_t index)->const _float3* const;

	auto AddObjects(std::vector<MapManager::MapEntity> objects)->void;
	auto GetObjects()const->std::vector<std::shared_ptr<MapObject>>;


private:
	const int32_t			_map_id;
	std::vector<_float3>	_spawn_points;
	std::vector<_float3>	_region_points;
	std::vector<std::shared_ptr<MapObject>>	_objects;
	std::map<int64_t, std::shared_ptr<GameSession>> _characters;
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
