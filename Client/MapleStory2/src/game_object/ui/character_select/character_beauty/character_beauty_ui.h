#pragma once
#include "src/utility/game_objects/game_object.h"

class CharacterCreateReturnBtn;
class CharacterBeautyItemList;
class CharacterBeautySelectSex;
class CharacterJobBtn;
class CharacterCreateBtn;
class CharacterSelectItem;
class Shader;
class ViBufferRect;
class Texture;

class CharacterBeautyUi final : public GameObject
{
public:
	explicit CharacterBeautyUi(const ComPtr<IDirect3DDevice9>& device);
	~CharacterBeautyUi() override = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<CharacterBeautyUi>;

public:
	enum class kBeautyStage
	{
		kSexChangeMan,
		kSexChangeGirl,
		kSexMan,
		kSexGirl,
		kSelectInit,
		kEnd
	};
	auto GetSexState()const->bool;

	auto GetBeautyStage() const ->kBeautyStage;
	auto ChangeBeautyStage(kBeautyStage stage)->void;

private:
	struct CharacterBeautyUiInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto AddComponents()->HRESULT;
	auto CreateSexButton()->HRESULT;
	auto CreateItemList()->HRESULT;

	auto SexBtnTick(double timeDelta)->HRESULT;
	auto GetCreateNameButton() const ->std::shared_ptr<CharacterCreateReturnBtn>;
private:
	std::shared_ptr<Texture>										_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>									_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>											_shader_com = nullptr;
	std::shared_ptr<Texture>										_texture_background = nullptr;

	bool															_sex_select = false;
	std::shared_ptr<CharacterBeautySelectSex>						_sex_btn[2];

	std::vector<std::shared_ptr<CharacterBeautyItemList>>			_item_list;

	std::shared_ptr<CharacterCreateReturnBtn>						_return_btn;

	kBeautyStage										_beauty_stage = kBeautyStage::kSexChangeMan;

	_matrix					_proj_matrix;
	CharacterBeautyUiInfo	_info;
};

