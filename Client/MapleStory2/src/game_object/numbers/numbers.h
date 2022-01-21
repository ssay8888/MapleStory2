#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class Numbers : public GameObject
{
public:
	Numbers();
	~Numbers() override = default;

struct NumberInfo
{
	bool is_player_attack;
	int64_t numbers;
	std::shared_ptr<Transform> parent_transform;
};

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create()->std::shared_ptr<Numbers>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Texture>							_player_attack_numbers = nullptr;
	std::shared_ptr<Texture>							_damage_numbers = nullptr;
	std::shared_ptr<Transform>							_transform_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;

	NumberInfo					_info;
	std::vector<int32_t>		_numbers;
	float						_start_pos;
	float						_end_pos;
	uint64_t					_start_time;
};

