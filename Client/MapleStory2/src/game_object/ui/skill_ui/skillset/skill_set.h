#pragma once
#include "src/utility/game_objects/game_object.h"

class PopupInfo;
class InputDevice;
class SkillLevelChangeBtn;
class Shader;
class ViBufferRect;
class Texture;

class SkillSet final : public GameObject
{
public:
	SkillSet(int32_t skillId, const _float3 pos);
	~SkillSet() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Tick(_float3 pos, double timeDelta) -> HRESULT;
	auto LateTick(_float3 pos, double timeDelta) -> HRESULT;
	auto Render() -> HRESULT override;
	auto Render(_float3 pos, std::shared_ptr<Shader>) -> HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create(int32_t skillId, _float3 pos)->std::shared_ptr<SkillSet>;
	auto IsCollision(_float3 pos)->bool;
	auto GetSkillIcon() const ->std::shared_ptr<Texture>;
	auto GetSkillId()const->int32_t;
	auto GetPopupInfo()const->std::shared_ptr<PopupInfo>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>							_skill_icon = nullptr;
	std::shared_ptr<Texture>							_skillset_texture = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;

	std::shared_ptr<SkillLevelChangeBtn>				_change_btn[2];
	std::shared_ptr<PopupInfo>							_popup_info_com;

	std::shared_ptr<InputDevice>						_input_device;
	uint8_t												_is_lbutton_down = 0;
	uint8_t												_is_lbutton_up = 0;

	_matrix		_proj_matrix;
	int32_t		_skill_id;
	bool		_is_select_skill;
	_float3		_pos, _size;
};


