#pragma once
#include "src/utility/game_objects/game_object.h"

class CharacterJobBtn;
class CharacterCreateBtn;
class CharacterSelectItem;
class Shader;
class ViBufferRect;
class Texture;

class CharacterSelectUi final : public GameObject
{
public:
	explicit CharacterSelectUi(const ComPtr<IDirect3DDevice9>& device);
	~CharacterSelectUi() override = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<CharacterSelectUi>;

public:
	enum class kCharacterSelectState
	{
		kSelectInit,
		kSelect,
		kCreateJob,
		kBeautyInit,
		kBeauty
	};
	auto GetState()const->kCharacterSelectState;
	auto ChangeState(kCharacterSelectState state)->void;

private:
	auto SelectUpdateTick(double timeDelta)->HRESULT;
	auto CreateJobUpdateTick(double timeDelta)->HRESULT;

	auto SelectRender()->HRESULT;
	auto SelectJobRender()->HRESULT;

private:
	auto AddComponents()->HRESULT;
	auto CreateSelectList()->HRESULT;
	auto CreateStartBtnList()->HRESULT;
	auto CreateJobBtnList()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<CharacterCreateBtn>					_create_btn;
	std::vector<std::shared_ptr<CharacterSelectItem>>	_character_list;
	std::vector<std::shared_ptr<CharacterJobBtn>>		_character_job_list;
	std::shared_ptr<Texture>							_texture_background = nullptr;


	kCharacterSelectState	_state{ kCharacterSelectState::kSelect };
	_matrix					_proj_matrix;
	int32_t					_select_item_index{};
	int32_t					_select_job_index{};
	float					_x, _y, _size_x, _size_y;

private:
	const int32_t _max_character_slot = 7;
};

