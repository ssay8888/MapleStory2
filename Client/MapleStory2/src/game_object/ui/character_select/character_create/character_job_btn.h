#pragma once
#include "character_create_job_btn.h"
#include "src/utility/game_objects/game_object.h"

class CharacterCreateJobBtn;
class ViBufferRect;
class Texture;
class Shader;

class CharacterJobBtn final : public GameObject
{
public:
	explicit CharacterJobBtn();
	~CharacterJobBtn() = default;

public:
	struct CreateJobBtnInfo
	{
		_float3 pos;
		_float3 size;
		int32_t job_id{};
		std::wstring job_name;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CreateJobBtnInfo* arg)->std::shared_ptr<CharacterJobBtn>;

public:
	enum kCreateBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};
	auto IsCollision() const ->bool;
	auto ChangeState(kCreateBtnState state)->void;
	auto GetCreateBtnState()const->CharacterCreateJobBtn::kCreateJobBtnState;
private:
	auto NormalRender(std::shared_ptr<Shader> shader)->HRESULT;
	auto OverRender(std::shared_ptr<Shader> shader)->HRESULT;
	auto SelectRender(std::shared_ptr<Shader> shader)->HRESULT;

private:
	auto AddComponents()->HRESULT;
private:
	CreateJobBtnInfo						_info;
	kCreateBtnState							_state;
	std::shared_ptr<Texture>				_select_texture;
	std::shared_ptr<Texture>				_select_texture_front;
	std::shared_ptr<Texture>				_select_texture_effect;
	std::shared_ptr<Texture>				_select_texture_showinfo;
	std::shared_ptr<Texture>				_select_texture_il;
	std::shared_ptr<ViBufferRect>			_vi_buffer_com = nullptr;
	std::shared_ptr<CharacterCreateJobBtn>	_create_job_btn;

	_matrix				_proj_matrix;
};

