#pragma once
#include "src/utility/game_objects/game_object.h"
class Shader;
class ViBufferRect;
class Texture;

class CharacterOkBtn final : public GameObject
{
public:
	explicit CharacterOkBtn();
	~CharacterOkBtn() = default;

public:
	struct OkBtnInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(OkBtnInfo* arg)->std::shared_ptr<CharacterOkBtn>;

public:
	auto IsCollision() const ->bool;
	auto SelectCharacter(int32_t index)const->bool;

private:
	enum kOkBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto AddComponents()->HRESULT;
	auto DrawTextContents() const ->void;

private:
	OkBtnInfo					_info;
	kOkBtnState					_state;
	std::shared_ptr<Texture>		_select_texture[kEnd];
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	_matrix				_proj_matrix;
};

