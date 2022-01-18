#pragma once
#include "src/utility/game_objects/game_object.h"
class Shader;
class ViBufferRect;
class Texture;

class CharacterItem final : public GameObject
{
public:
	explicit CharacterItem();
	~CharacterItem() = default;

public:
	struct CreateItemInfo
	{
		_float3		pos;
		_float3		size;
		int32_t		man_item_id;
		int32_t		girl_item_id;
		kEqpType	type;
		bool		sex;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateItemInfo* arg)->std::shared_ptr<CharacterItem>;

public:
	enum kCreateItemState
	{
		kNormal,
		kSelect,
		kOver,
		kEnd
	};
	auto IsCollision() const ->bool;
	auto GetButtonState() const->kCreateItemState;
	auto ChangeState(kCreateItemState state)->void;

	auto GetManItemId()const->int32_t;
	auto GetGirlItemId()const->int32_t;

private:
	auto AddComponents()->HRESULT;

private:
	CreateItemInfo					_info;
	kCreateItemState				_state;
	std::shared_ptr<Texture>		_texture;
	std::shared_ptr<Texture>		_texture_icon;
	ComPtr<IDirect3DTexture9>		_texture_face_icon;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	_matrix				_proj_matrix;
};

