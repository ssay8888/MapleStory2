#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class CharacterSelectItem : public GameObject
{
public:
	explicit CharacterSelectItem();
	~CharacterSelectItem() = default;

public:
	struct SelectItemInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(SelectItemInfo* arg)->std::shared_ptr<CharacterSelectItem>;

	auto IsCollision()->bool;

public:
	enum kSelectItemState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto ChangeState(kSelectItemState state)->void;
private:
	auto AddComponents()->HRESULT;
private:

	SelectItemInfo					_info;
	std::shared_ptr<Texture>		_select_texture[kEnd];
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	kSelectItemState				_state = kNormal;


	_matrix				_proj_matrix;
};

