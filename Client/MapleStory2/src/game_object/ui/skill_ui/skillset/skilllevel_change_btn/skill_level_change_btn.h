#pragma once
#include "src/utility/game_objects/game_object.h"

class InputDevice;
class Texture;
class ViBufferRect;
class Shader;

class SkillLevelChangeBtn final : public GameObject
{
	enum kBtnState
	{
		kDisable,
		kEnable,
		kPush,
		kOver
	};
public:
	SkillLevelChangeBtn(std::function<void()> callback, bool isMinus);
	~SkillLevelChangeBtn() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto Tick(_float3 pos, double timeDelta) -> HRESULT;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Render(_float3 pos, std::shared_ptr<Shader> shader) -> HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create(std::function<void()> callback, bool isMinus)->std::shared_ptr<SkillLevelChangeBtn>;

private:
	auto AddComponents()->HRESULT;
	auto IsCollision(_float3 pos)->bool;
	auto ButtonStateChange(_float3 pos)->void;

private:
	std::shared_ptr<InputDevice>						_input_device;
	std::function<void()>								_callback;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Texture>							_texture_com = nullptr;

	bool					_is_minus;
	_matrix					_proj_matrix;
	kBtnState				_btn_state;
	uint8_t					_is_lbutton_down = 0;
	uint8_t					_is_lbutton_up = 0;

};

