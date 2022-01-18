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
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<PlayerInfo>;
	
private:
	auto AddComponents()->HRESULT;

	auto HpMpRender()->HRESULT;
	auto HpRender()->HRESULT;
	auto MpRender()->HRESULT;
	auto ExpRender()->HRESULT;
	auto ExpProgressRender()->HRESULT;
	auto SubFrameRender()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<Texture>							_texture_hp = nullptr;
	std::shared_ptr<Texture>							_texture_mp = nullptr;
	std::shared_ptr<Texture>							_texture_exp_frame= nullptr;
	std::shared_ptr<Texture>							_texture_exp_progress= nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Shader>								_shader_gage = nullptr;

	_matrix					_proj_matrix;
	float					_x{}, _y{}, _size_x{}, _size_y{};
};

