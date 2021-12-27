#pragma once
#include "src/utility/game_objects/game_object.h"

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
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

public:
	struct PlayerInfo
	{
		_float3 pos;
		bool	sex;
		std::shared_ptr<Character> character;
	};
	auto GetCurrentDynamicMesh()->std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>;
	auto GetInfo() const->PlayerInfo;
	auto ChangeEqp(GameContents::kEquipeType type, int32_t itemId)->void;
	auto GetEqpList() const->std::shared_ptr<Equipped>;

private:
	auto AddComponents()->HRESULT;
	auto SetUpConstantTable() const->HRESULT;

	auto GravityPlayer(double timeDelta)->void;
	auto StraightCheck()->bool;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Player>;
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;

private:
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
	std::shared_ptr<Transform>						_transform_com = nullptr;
	std::shared_ptr<Collider>						_character_shose_aabb_com = nullptr;
	std::shared_ptr<Collider>						_character_body_aabb_com = nullptr;
	std::shared_ptr<Collider>						_block_ragne_aabb_com = nullptr;
	std::shared_ptr<Collider>						_reload_ragne_aabb_com[kReloadEnd];
	std::shared_ptr<Shader>							_shader_com = nullptr;
	std::vector<std::shared_ptr<MeshDynamic>>		_character_mesh_list;
	std::shared_ptr<Equipped>						_eqp_list;
	std::map<int32_t, std::shared_ptr<MeshDynamic>>	_eqp_mesh;
	std::vector<std::shared_ptr<MapObject>>			_map_objects;
	std::shared_ptr<MapObject>						_last_tile_map_object;
	std::shared_ptr<MapObject>						_last_wall_map_object;


	bool				_is_idle = true;
	int32_t				_current_mesh_num = 0;
	int32_t				_new_mesh_num = 0;
	_float3				_target_pos;
	PlayerInfo			_info;
};

