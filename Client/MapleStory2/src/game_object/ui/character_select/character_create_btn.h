#pragma once
#include "src/utility/game_objects/game_object.h"
class Shader;
class ViBufferRect;
class Texture;

class CharacterCreateBtn final : public GameObject
{
public:
	explicit CharacterCreateBtn();
	~CharacterCreateBtn() = default;

public:
	struct CreateBtnInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const ->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateBtnInfo* arg)->std::shared_ptr<CharacterCreateBtn>;

	auto IsCollision() const ->bool;

private:
	enum kCreateBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto AddComponents()->HRESULT;
	auto DrawTextContents() const ->void;

private:
	CreateBtnInfo					_info;
	kCreateBtnState					_state;
	std::shared_ptr<Texture>		_select_texture[kEnd];
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;
	
	_matrix				_proj_matrix;
};

