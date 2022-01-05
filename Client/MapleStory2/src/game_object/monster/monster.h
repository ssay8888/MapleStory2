#pragma once
#include "data_reader/data_reader_manager.h"
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

	enum class kMonsterState
	{
		kIdleA,
		kWalkA,
		kBoreA,
		kRegenA,
		kRunA,
		kDeadA,
		kDeadB,
		kAttackA,
		kAttackB,
		kAttackC,
		kDamgA,
		kDamgB,
		kDamgC,
	};

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto LateTick(double timeDelta) -> int32_t override;
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
	auto GetMonsterInfo()const->std::shared_ptr<DataReaderManager::MonsterInfo>;
	auto GetAnimationPeriod(kMonsterState type)->double;
	auto GetAnimationTimeAcc()->double;
	auto GetStateIndex(kMonsterState state)->int32_t;

	auto PlayAnimation(double timeDelta)->void;

	auto ChangeAnimation(kMonsterState index)->void;
	auto ChangeMonsterState(int32_t index)->void;

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

	std::shared_ptr<DataReaderManager::MonsterInfo>						_monster_info;
	std::shared_ptr<MonsterStats>										_monster_stat;

	std::map<int32_t, std::shared_ptr<MonsterState>>					_monster_states;
	std::map<kMonsterState, int32_t>									_state_index;
	std::map<kMonsterState, int32_t>									_animaion_index;
	std::shared_ptr<MonsterState>										_current_monster_state;


};

