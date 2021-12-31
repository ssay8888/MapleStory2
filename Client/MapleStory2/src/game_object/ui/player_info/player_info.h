#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class PlayerInfo final : public GameObject
{
public:
	explicit PlayerInfo(const ComPtr<IDirect3DDevice9>& device);
	~PlayerInfo() override = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<PlayerInfo>;
	
private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;

	_matrix					_proj_matrix;
	float					_x, _y, _size_x, _size_y;
};

