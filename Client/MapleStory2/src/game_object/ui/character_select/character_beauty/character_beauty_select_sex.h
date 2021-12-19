#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferRect;
class Texture;
class Shader;

class CharacterBeautySelectSex final : public GameObject
{
public:
	explicit CharacterBeautySelectSex();
	~CharacterBeautySelectSex() = default;

public:
	struct CreateSexBtnInfo
	{
		_float3 pos;
		_float3 size;
		bool	sex;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateSexBtnInfo* arg)->std::shared_ptr<CharacterBeautySelectSex>;

public:
	enum kCreateSexBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto IsCollision() const ->bool;
	auto GetButtonState() const->kCreateSexBtnState;
	auto ChangeButtonState(kCreateSexBtnState state)->void;


private:
	auto AddComponents()->HRESULT;

private:
	CreateSexBtnInfo				_info;
	kCreateSexBtnState				_state;
	std::shared_ptr<Texture>		_select_texture;
	std::shared_ptr<Texture>		_sex_texture;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	_matrix				_proj_matrix;
};
