#pragma once
#include "src/utility/game_objects/game_object.h"

class CharacterState;
class MapObject;
class Collider;
class Equipped;
class Character;
class Shader;
class MeshDynamic;
class Texture;
class ViBufferRect;
class Transform;

class Player final : public GameObject
{
public:
	explicit Player(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Player() = default;


public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;

public:
	struct PlayerInfo
	{
		_float3 pos;
		bool	sex;
	};

	auto GetTransform()const->std::shared_ptr<Transform>;
	auto GetCurrentDynamicMesh()->std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>;
	auto GetInfo() const->PlayerInfo;
	auto GetCharacterColliderAabb()const->std::shared_ptr<Collider>;
	auto GetReloadRangeAabb()const->std::vector<std::shared_ptr<Collider>>;
	auto GetBlockRangeAabb()const->std::shared_ptr<Collider>;

	auto GetStartAttackTime()const->uint64_t;
	auto IsStartAttackTime()const->bool;
	auto SettingStartAttackTime()->void;
	auto ResetStartAttackTime()->void;

	auto GetRadian()const->float;
	auto SetRadian(float radian)->void;

	auto GetCurrentAnimationType()const->kAnimationType;

	auto GetAnimationTimeAcc()->double;




	auto ChangeEqp(GameContents::kEquipeType type, int32_t itemId)->void;
	auto GetEqpList() const->std::shared_ptr<Equipped>;
	auto ChangeCharacterState(const std::shared_ptr<CharacterState>& state)->void;
	auto GetCharacterState()const->std::shared_ptr<CharacterState>;
	auto ChangeAnimation(kAnimationType type)->void;
	auto PlayAnimation(const double timeDelta)->void;
	auto SetPlayAnimation(const double timeDelta)->void;

private:
	auto AddComponents()->HRESULT;
	auto SetUpConstantTable() const->HRESULT;

	auto OpenInventory()->HRESULT;
	auto MapChange()->HRESULT;
	auto SpRecovery()->void;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Player>;
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;
	enum kReloadRegenAabb
	{
		kReloadUp,
		kReloadDown,
		kReloadLeft,
		kReloadRight,
		kReloadFront,
		kReloadBack,
		kReloadEnd
	};

private:
	std::shared_ptr<Transform>						_transform_com = nullptr;
	std::shared_ptr<Collider>						_character_aabb_com = nullptr;
	std::shared_ptr<Collider>						_block_ragne_aabb_com = nullptr; // 블록가져올 범위
	std::vector<std::shared_ptr<Collider>>			_reload_ragne_aabb_com;
	std::shared_ptr<Shader>							_shader_com = nullptr;
	std::vector<std::shared_ptr<MeshDynamic>>		_character_mesh_list;
	std::shared_ptr<Equipped>						_eqp_list;
	std::map<int32_t, std::shared_ptr<MeshDynamic>>	_eqp_mesh;
	std::vector<std::shared_ptr<MapObject>>			_map_objects;
	std::shared_ptr<CharacterState>					_character_state;

	bool				_is_idle = true;
	uint64_t			_start_attack_time = 0;
	float				_radian = 0.f;
	int32_t				_current_mesh_num = 0;
	int32_t				_new_mesh_num = 0;
	_float3				_target_pos;
	PlayerInfo			_info;
	uint64_t			_last_sp_recovery;
};

