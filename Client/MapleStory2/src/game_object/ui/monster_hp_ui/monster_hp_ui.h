#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class Texture;
class ViBufferRect;
class Monster;

class MonsterHpUi final : public GameObject
{
public:
	MonsterHpUi();
	~MonsterHpUi() override = default;

	static auto GetInstance()->std::shared_ptr<MonsterHpUi>
	{
		static auto instance = std::make_shared<MonsterHpUi>();
		return instance;
	}

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Render(std::shared_ptr<Shader> shader) -> HRESULT;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto SetMonster(std::shared_ptr<Monster> monster)->void;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Monster>							_monster = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Texture>							_texture_frame_com = nullptr;
	std::shared_ptr<Texture>							_texture_hp_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;


	uint64_t				_last_time;
	_matrix					_proj_matrix;
	_float3					_size;
	_float3					_pos;
};

