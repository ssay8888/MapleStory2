#pragma once
#include "protocol/game_enum.pb.h"
#include "src/utility/game_objects/game_object.h"

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
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Render(const _float3& pos, std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto SelectBtn()->bool;

public:
	auto SetParentPos(_float3 pos)->void;
	auto SetSelect(bool click)->void;
	auto IsSelect()->bool;

public:
	static auto Create(Protocol::kInventoryType type, _float3 pos)->std::shared_ptr<InventoryTabBtn>;

private:
	auto AddComponents()->HRESULT;

private:
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

