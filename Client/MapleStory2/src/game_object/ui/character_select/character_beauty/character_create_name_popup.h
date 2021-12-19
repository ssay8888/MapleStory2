#pragma once
#include "src/game_object/ui/login/text_box_ui.h"
class Shader;
class ViBufferRect;
class Texture;

class CharacterCreateNamePopup final : public TextBoxUi
{
public:
	explicit CharacterCreateNamePopup();
	~CharacterCreateNamePopup() = default;

public:
	struct CharacterCreateNamePopupInfo
	{
		_float3 popup_pos;
		_float3 popup_size;
		TextBoxInformation textbox_info{};
		bool sex;
	};
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(std::shared_ptr<Shader> shader)->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(CharacterCreateNamePopupInfo* arg)->std::shared_ptr<CharacterCreateNamePopup>;

public:
	enum kCreateJobBtnState
	{
		kNormal,
		kOver,
		kSelect,
		kEnd
	};

	auto IsCollision() const ->bool;
	auto GetButtonState() const->kCreateJobBtnState;
	auto IsShow()const->bool;
	auto ChangeShow(bool show)->void;

private:
	auto AddComponents()->HRESULT;

	auto IsOkCollision() const ->bool;
	auto IsCancelCollision()->bool;
	auto CreateCharacter()->void;
	auto SendCreateCharacter(std::string name, int32_t gender, int32_t coatIndex, int32_t pantsIndex, int32_t faceIndex, int32_t shoesIndex) const->void;

private:
	CharacterCreateNamePopupInfo	_info;
	kCreateJobBtnState				_state;
	std::shared_ptr<Texture>		_texture;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	_matrix				_proj_matrix;
	bool				_is_show = false;
};
