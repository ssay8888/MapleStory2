#pragma once
#include "src/utility/game_objects/game_object.h"

class ViBufferRect;
class Texture;
class Shader;

class CharacterCreateReturnBtn final : public GameObject
{
public:
	explicit CharacterCreateReturnBtn();
	~CharacterCreateReturnBtn() = default;

public:
	struct CreateNameBtnInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateNameBtnInfo* arg)->std::shared_ptr<CharacterCreateReturnBtn>;

	enum kCreateBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto GetButtonState() const ->kCreateBtnState;
	auto IsCollision() const ->bool;

private:
	auto AddComponents()->HRESULT;
private:
	CreateNameBtnInfo				_info;
	kCreateBtnState					_state;
	std::shared_ptr<Texture>		_select_texture;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	_matrix				_proj_matrix;
};
