#pragma once
#include "src/utility/game_objects/game_object.h"

class InputDevice;
class SkillSet;
class Shader;
class ViBufferRect;
class Texture;

class SkillUi final : public GameObject
{
public:
	explicit SkillUi();

	~SkillUi() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto ChangeShow()->void;
	auto GetSkillSet(int32_t skillId)->std::shared_ptr<SkillSet>;

public:
	static auto Create()->std::shared_ptr<SkillUi>;

private:
	auto AddComponents()->HRESULT;
	auto CloseButtonTick(double timeDelta)->HRESULT;
	auto MoveInventoryFrame()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Texture>							_inventory_close_btn = nullptr;
	std::shared_ptr<InputDevice>						_input_device;

	std::map<int32_t, std::shared_ptr<SkillSet>>		_skill_sets;


	_matrix					_proj_matrix;
	_float3					_pos, _size;
	bool					_close_btn_over;
	bool					_is_show = false;
	bool					_moving = false;
	uint8_t					_is_lbutton_down = 0;
	uint8_t					_is_lbutton_up = 0;
};

