#pragma once
#include "data_reader/data_reader_manager.h"
#include "protocol/game_protocol.pb.h"
#include "src/utility/game_objects/game_object.h"

class MonsterState;
class Shader;
class MeshDynamic;
class MapObject;
class Collider;
class MonsterStats;

class Monster final : public GameObject
{
public:
	explicit Monster(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Monster() = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Monster>;

public:
	auto GetTransform() const ->std::shared_ptr<Transform>;
	auto GetMonsterColliderObb()const->std::shared_ptr<Collider>;
	auto GetMonsterColliderAabb()const->std::shared_ptr<Collider>;
	auto GetReloadRangeAabb()const->std::vector<std::shared_ptr<Collider>>;
	auto GetBlockRangeAabb()const->std::shared_ptr<Collider>;
	auto GetMonsterInfo()const->Protocol::GameServerRespawnMonster;
	auto GetAnimationPeriod(Protocol::kMonsterState type)->double;
	auto GetAnimationTimeAcc()->double;
	auto GetStateIndex(Protocol::kMonsterState state)->int32_t;
	auto GetMonsterState(Protocol::kMonsterState state)->std::shared_ptr<MonsterState>;
	auto GetCurrentState()const->std::shared_ptr<MonsterState>;
	auto GetStat()const->std::shared_ptr<MonsterStats>;

	auto PlayAnimation(double timeDelta)->void;

	auto ChangeAnimation(Protocol::kMonsterState index)->void;
	auto ChangeMonsterState(int32_t index)->bool;
	auto ChangeSkillId(int32_t id)->void;
	auto GetSkillId()const->int32_t;

private:
	auto AddComponents()->HRESULT;
	auto AnimationLoad()->int32_t;
	auto SetUpConstantTable() const->HRESULT;
	auto GetMonster()->std::shared_ptr<Monster>;

private:
	std::shared_ptr<Transform>											_transform_com = nullptr;
	std::shared_ptr<Collider>											_obb_com = nullptr;
	std::shared_ptr<Collider>											_aabb_com = nullptr;
	std::shared_ptr<Collider>											_block_range_aabb_com = nullptr; // 블록가져올 범위
	std::vector<std::shared_ptr<Collider>>								_reload_range_aabb_com;
	std::shared_ptr<Shader>												_shader_com = nullptr;
	std::map<int32_t, std::shared_ptr<MeshDynamic>>						_mesh_list; 
	std::vector<std::shared_ptr<MapObject>>								_map_objects;

	Protocol::GameServerRespawnMonster									_monster_info;
	std::shared_ptr<MonsterStats>										_monster_stat;

	std::map<int32_t, std::shared_ptr<MonsterState>>					_monster_states;
	std::map<Protocol::kMonsterState, int32_t>							_state_index;
	std::map<Protocol::kMonsterState, int32_t>							_animaion_index;
	std::shared_ptr<MonsterState>										_current_monster_state;
	int32_t	_skill_id;


};

