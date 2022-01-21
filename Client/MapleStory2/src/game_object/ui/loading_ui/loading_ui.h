#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class LoadingUi final : public GameObject
{
public:
	LoadingUi();
	~LoadingUi() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create()->std::shared_ptr<LoadingUi>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_progress = nullptr;
	std::shared_ptr<Texture>							_loading_ui = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;

	_float3		_pos;
	_float3		_size;
	_matrix		_proj_matrix;
};

