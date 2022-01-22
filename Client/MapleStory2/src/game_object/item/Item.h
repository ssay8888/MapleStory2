#pragma once
#include "protocol/game_enum.pb.h"
#include "src/utility/game_objects/game_object.h"

class PopupInfo;
class Shader;
class InputDevice;
class ViBufferRect;
class Texture;

class Item final : public GameObject
{
public:
	struct ItemInfo
	{
		int32_t item_id;
		int32_t quantity;
		int32_t position;
		int32_t str;
		int32_t dex;
		int32_t int_;
		int32_t luk;
		int32_t wap;
		Protocol::kInventoryType type;
	};

	Item(ItemInfo info);
	~Item() override = default;
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto Tick(const _float3& pos, double timeDelta)->HRESULT;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Render(const _float3& pos, std::shared_ptr<Shader> shader) const->HRESULT;
	auto RenderEquipped(const _float3& pos, std::shared_ptr<Shader> shader) const->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;

public:
	auto SearchMouseOverState(const _float3& pos)->bool;
	auto SearchMouseOverStateEquipped(const _float3& pos)->bool;
	auto SelectItem(const _float3& pos, uint8_t isLButtonDown, uint8_t isLButtonUp) const ->bool;
	auto UnSelectItem(const _float3& pos, uint8_t isLButtonDown, uint8_t isLButtonUp) const ->bool;
	auto SetSelectItem(bool select)->void;
	auto IsSelectItem()const->bool;

	auto MouseOverState()const->bool;
	auto SetIsMouseUp(bool isup)->void;

	auto FindPosition(const _float3& pos)->int32_t;

	auto GetPosition()const->int32_t;

	auto ChangePosition(int32_t position)->void;
	auto ResetPosition()->void;

	auto GetPopupInfo()const->std::shared_ptr<PopupInfo>;

	auto GetItemId()const->int32_t;

	auto GetItemQuantity()const->int32_t;
	auto SetItemQuantity(int32_t value)->void;

	auto GetInventoryType()const->Protocol::kInventoryType;

	auto GetItemIcon()const->std::shared_ptr<Texture>;

public:
	static auto Create(ItemInfo info)->std::shared_ptr<Item>;

private:
	HRESULT AddComponents();

private:
	ItemInfo _info;
	_float3 _original_pos{ -81, 154, 0 };

	std::shared_ptr<Texture>		_texture_com = nullptr;
	std::shared_ptr<InputDevice>	_input_device = nullptr;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;
	std::shared_ptr<PopupInfo>		_popup_info_com = nullptr;


	bool _is_select_item = false;
	bool _is_up_mouse = false;
	_float3 _pos;

	_matrix _proj_matrix;
};

