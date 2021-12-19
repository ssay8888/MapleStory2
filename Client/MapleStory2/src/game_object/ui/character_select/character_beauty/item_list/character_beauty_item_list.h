#pragma once
#include "src/utility/game_objects/game_object.h"

class CharacterItem;
class Shader;
class ViBufferRect;
class Texture;

class CharacterBeautyItemList final : public GameObject
{
public:
	explicit CharacterBeautyItemList();
	~CharacterBeautyItemList() = default;

public:
	struct CreateItemListTextureInfo
	{
		_float3					pos;
		_float3					size;
		std::vector<int32_t>    man_items;
		std::vector<int32_t>	girl_items;
		bool					sex;
		kEqpType				type;
		std::wstring			title_name;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateItemListTextureInfo* arg)->std::shared_ptr<CharacterBeautyItemList>;

public:
	auto InitEqp()->void;
	
private:
	auto AddComponents()->HRESULT;

private:
	CreateItemListTextureInfo					_info;
	std::shared_ptr<Texture>					_texture;
	std::shared_ptr<ViBufferRect>				_vi_buffer_com = nullptr;
	std::vector<std::shared_ptr<CharacterItem>>	_items;

	int32_t										_select_index;
	_matrix				_proj_matrix;
};
