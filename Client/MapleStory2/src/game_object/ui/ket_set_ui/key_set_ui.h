#pragma once
#include "src/utility/game_objects/game_object.h"

class SkillSet;
class Item;
class InputDevice;
class Shader;
class ViBufferRect;
class Texture;

class KeySetUi final : public GameObject
{
public:
	KeySetUi();
	~KeySetUi() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) -> HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto SetItem(std::shared_ptr<Item> item)->void;

	auto SetSkill(std::shared_ptr<SkillSet> skill)->void;
	auto GetSkill()const->std::shared_ptr<SkillSet>;

	auto GetKey()const->int32_t;

	auto IsCollision()->bool;

public:
	static auto Create(_float3 pos, int32_t index, int32_t key)->std::shared_ptr<KeySetUi>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<InputDevice>						_input_device;
	std::shared_ptr<Texture>							_free_set_ui = nullptr;
	std::shared_ptr<Item>								_item = nullptr;
	std::shared_ptr<SkillSet>							_skill = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;

	int32_t _index;
	int32_t _key;
	_float3 _pos;
	_matrix _proj_matrix;

};

