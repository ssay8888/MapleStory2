#pragma once
#include "src/utility/game_objects/game_object.h"

class MeshDynamic;
class Shader;
class MeshStatic;
class Renderer;

class Coat final : public GameObject
{
public:
	Coat(const ComPtr<IDirect3DDevice9>& device);
	~Coat() override = default;
public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Coat>;
	
public:
	auto GetMesh()->std::shared_ptr<MeshDynamic>;

private:
	HRESULT AddComponents();
	HRESULT SetUpConstantTable();

private:
	std::shared_ptr<Renderer>		_renderer_com = nullptr;
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<MeshDynamic>	_mesh_com = nullptr;
	std::shared_ptr<Shader>			_shader_com = nullptr;
	std::pair<std::shared_ptr<MeshDynamic>, std::shared_ptr<MeshDynamic>>	_player_current_mesh;

	_matrix				_origin_matrix;
	const _matrix* _parent_bone_matrix;
	const _matrix* _parent_world_matrix;
};

