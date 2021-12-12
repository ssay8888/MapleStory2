#pragma once
#include "src/utility/game_objects/game_object.h"

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

private:
	struct CharacterBeautyUiInfo
	{
		_float3 pos;
		_float3 size;
	};
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>							_texture_com = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;
	std::shared_ptr<Texture>							_texture_background = nullptr;


	_matrix					_proj_matrix;
	CharacterBeautyUiInfo	_info;
};

