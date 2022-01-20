#pragma once
#include "src/game_object/item/Item.h"
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class PopupInfo final : public GameObject
{
public:
	PopupInfo();
	~PopupInfo() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto SetItemTexture(std::shared_ptr<Texture> itemTexture)->void;
	auto SetComment(std::wstring str)->void;

	auto SetItemInfo(Item::ItemInfo itemInfo)->void;

public:
	auto AddComponents()->HRESULT;
	static auto Create()->std::shared_ptr<PopupInfo>;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<Texture>							_item_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	ComPtr<ID3DXFont>									_font = nullptr;

	_matrix					_proj_matrix;
	float					_size_x{}, _size_y{};
	_float3					_pos;
	Item::ItemInfo			_info;
	std::wstring			_comment;
};

