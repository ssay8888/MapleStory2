#pragma once
#include "protocol/game_enum.pb.h"
#include "src/utility/game_objects/game_object.h"

class Item;
class InputDevice;
class Shader;
class ViBufferRect;
class Texture;

class InventoryTabBtn final : public GameObject
{
public:
	explicit InventoryTabBtn(Protocol::kInventoryType type, _float3 pos);
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto Tick(const _float3& pos, double timeDelta) -> int32_t;
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Render(const _float3& pos, std::shared_ptr<Shader> shader)->HRESULT;
	auto ItemListRender(const _float3& pos, std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto SelectBtn(uint8_t lButton)->bool;

public:
	auto SetParentPos(_float3 pos)->void;
	auto SetSelect(bool click)->void;
	auto IsSelect() const ->bool;

	auto AllItem() const ->std::map<int32_t, std::shared_ptr<Item>>;
	auto AddItem(int32_t position, std::shared_ptr<Item> item)->void;
	auto FindItem(int32_t position)->std::shared_ptr<Item>;
	auto RemoveItem(int32_t position)->void;
	auto GetFreeSlot()const->int32_t;

public:
	static auto Create(Protocol::kInventoryType type, _float3 pos)->std::shared_ptr<InventoryTabBtn>;

private:
	auto AddComponents()->HRESULT;

private:
	std::map<int32_t, std::shared_ptr<Item>>				_items;

	std::shared_ptr<Texture>				_inventory_tab_btn = nullptr;
	std::shared_ptr<ViBufferRect>			_vi_buffer_com = nullptr;
	std::shared_ptr<InputDevice>			_input_device;
	Protocol::kInventoryType _type;
	_float3 _pos;
	_float3 _parent_pos;
	_matrix _proj_matrix;
	bool _is_over = false;
	bool _is_click = false;
};

