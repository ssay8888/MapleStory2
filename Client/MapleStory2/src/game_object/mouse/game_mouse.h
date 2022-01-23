#pragma once
#include "src/utility/game_objects/game_object.h"

class InputDevice;
class ViBufferRect;
class Texture;
class Shader;

class GameMouse final : public GameObject
{
public:
	GameMouse();
	~GameMouse() override = default;
public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;
public:
	static auto Create()->std::shared_ptr<GameMouse>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Texture>							_click_mouse = nullptr;
	std::shared_ptr<Texture>							_grab_mouse = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<InputDevice>						_input_device = nullptr;

	_float3		_pos;
	_matrix		_proj_matrix;

	bool _is_click = false;

};

