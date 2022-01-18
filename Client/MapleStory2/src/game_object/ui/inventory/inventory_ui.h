#pragma once
#include "protocol/game_enum.pb.h"
#include "src/utility/game_objects/game_object.h"

class InputDevice;
class InventoryTabBtn;
class ViBufferRect;
class Shader;
class Texture;

class Inventory final : public GameObject
{
public:
	Inventory();
	~Inventory() override = default;
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto GetInventoryTab(Protocol::kInventoryType type)->std::shared_ptr<InventoryTabBtn>;
	auto ChangeShow()->void;

public:
	static auto Create()->std::shared_ptr<Inventory>;

private:
	auto AddComponents()->HRESULT;

	auto TabButtonTick(double timeDelta)->HRESULT;

	auto CloseButtonTick(double timeDelta)->HRESULT;
	auto MoveInventoryFrame()->HRESULT;

private:
	std::shared_ptr<Texture>				_inventory_frame = nullptr;
	std::shared_ptr<Texture>				_inventory_close_btn = nullptr;
	std::shared_ptr<Shader>					_shader_com = nullptr;
	std::shared_ptr<ViBufferRect>			_vi_buffer_com = nullptr;
	std::shared_ptr<InventoryTabBtn>		_inventory_tab_btn[Protocol::kInventoryType::kInventoryEnd];
	std::shared_ptr<InputDevice>			_input_device;
	_matrix _proj_matrix;
	_float3 _pos;

	bool _is_show;
	bool _close_btn_over;
	bool _moving;

	uint8_t _is_lbutton_down = 0;
	uint8_t _is_lbutton_up = 0;

	const float _size_x = 464;
	const float _size_y = 524;

};

