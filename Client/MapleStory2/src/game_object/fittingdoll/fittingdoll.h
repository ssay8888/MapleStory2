#pragma once
#include "src/utility/game_objects/game_object.h"

class Character;
class Equipped;
class MeshDynamic;
class Shader;

class Fittingdoll final : public GameObject
{
public:
	explicit Fittingdoll(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Fittingdoll() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

public:
	struct FittingdollInfo
	{
		_float3 pos;
		bool	sex;
		std::shared_ptr<Character> character;
	};
	auto GetCurrentDynamicMesh() ->std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>;
	auto GetInfo() const ->FittingdollInfo;
	auto ChangeEqp(GameContents::kEquipeType type, int32_t itemId)->void;
	auto GetEqpList() const ->std::shared_ptr<Equipped>;

private:
	auto AddComponents()->HRESULT;
	auto SetUpConstantTable() const->HRESULT;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Fittingdoll>;
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<GameObject> override;

private:
	std::shared_ptr<Transform>						_transform_com = nullptr;
	std::shared_ptr<Shader>							_shader_com = nullptr;
	std::vector<std::shared_ptr<MeshDynamic>>		_character_mesh_list;
	std::shared_ptr<Equipped>						_eqp_list;
	std::map<int32_t, std::shared_ptr<MeshDynamic>>	_eqp_mesh;
	

	bool				_is_idle = true;
	int32_t				_current_mesh_num = 0;
	int32_t				_new_mesh_num = 0;
	_float3				_target_pos;
	FittingdollInfo		_info;
};

