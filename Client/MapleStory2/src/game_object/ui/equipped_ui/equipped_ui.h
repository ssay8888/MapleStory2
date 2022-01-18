#pragma once
#include "protocol/game_enum.pb.h"
#include "src/utility/game_objects/game_object.h"

class StatupBtn;
class InventoryTabBtn;
class Inventory;
class InputDevice;
class ViBufferRect;
class Shader;
class Texture;

class EquippedUi final : public GameObject
{
public:
	EquippedUi();
	~EquippedUi() override = default;
public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;
	static auto Create()->std::shared_ptr<EquippedUi>;

public:
	auto ChangeShow()->void;
	auto MoveInventoryFrame()->HRESULT;
private:
	auto AddComponents()->HRESULT;
	auto CloseButtonTick(double timeDelta)->HRESULT;

	auto ItemInfoRender()->HRESULT;
	auto CharacterInfoRender()->HRESULT;
	auto CharacterStatRender()->HRESULT;

	auto UnEqpItem(std::shared_ptr<InventoryTabBtn> equippedTab, std::shared_ptr<Inventory> inventory, GameContents::kEquipeType type)->void;

	auto ColliderCoat()->bool;
	auto ColliderPants()->bool;
	auto ColliderShoes()->bool;
	auto ColliderWeapon()->bool;

private:
	std::shared_ptr<Texture>				_inventory_frame = nullptr;
	std::shared_ptr<Texture>				_inventory_close_btn = nullptr;
	std::shared_ptr<Shader>					_shader_com = nullptr;
	std::shared_ptr<ViBufferRect>			_vi_buffer_com = nullptr;
	std::shared_ptr<InputDevice>			_input_device;

	std::shared_ptr<StatupBtn>				_statup_btn[Protocol::kStatType::kStatEnd];

	bool _is_show;
	bool _close_btn_over = false;
	bool _moving;

	uint8_t _is_lbutton_down = 0;
	uint8_t _is_lbutton_up = 0;

	_matrix _proj_matrix;
	_float3 _pos;
	const float _size_x = 483;
	const float _size_y = 297;
};

