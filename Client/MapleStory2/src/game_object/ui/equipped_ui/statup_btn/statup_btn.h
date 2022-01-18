#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class InputDevice;
class ViBufferRect;
class Texture;

class StatupBtn final : public GameObject
{
public:
	StatupBtn();
	~StatupBtn() override = default;

enum kButtonState
{
	kDisable,
	kEnable,
	kOver,
	kDown
};
public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Tick(_float3 pos, double timeDelta)->HRESULT;
	auto LateTick(_float3 pos, double timeDelta)->HRESULT;
	auto Render(_float3 pos, std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

	static auto Create(std::function<void()> call)->std::shared_ptr<StatupBtn>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<InputDevice>			_input_device;
	std::shared_ptr<Texture>				_statup_btn = nullptr;
	std::shared_ptr<ViBufferRect>			_vi_buffer_com = nullptr;
	std::function<void()>					_call;
	kButtonState							_state;
	_matrix _proj_matrix;
};

