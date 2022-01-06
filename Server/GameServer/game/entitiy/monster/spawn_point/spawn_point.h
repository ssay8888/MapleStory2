#pragma once
class SpawnPoint : public std::enable_shared_from_this<SpawnPoint>
{
public:
	SpawnPoint() = default;
	~SpawnPoint() = default;

public:
	auto SetPosition(_float3 position)->void;
	auto GetPosition() const->_float3;

	auto SetSpawnNpcId(int32_t id)->void;
	auto GetSpawnNpcId() const->int32_t;
private:
	_float3 _position;
	int32_t _spawn_npc_id;
};

