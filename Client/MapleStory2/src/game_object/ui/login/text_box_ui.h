#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class TextBoxUi : public GameObject
{
public:
	explicit TextBoxUi(const ComPtr<IDirect3DDevice9>& device);
	~TextBoxUi() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) ->HRESULT override;
	auto LateTick(double timeDelta) ->HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;
	static auto Create()->std::shared_ptr<TextBoxUi>;

public:
	auto ClearText()->void;
	auto AddText(const std::wstring& str)->void;
	auto SetText(const std::wstring& str)->void;
	auto GetText() const ->std::wstring;
	auto SetFocus(bool focus)->void;
	auto IsFocus() const ->bool;

protected:
	auto UiDrawText()->void;
	auto AddComponents()->HRESULT;


protected:
	RECT							_rc;
	bool							_is_focus;
	bool							_is_password;
	_float2							_size;
	_float2							_pos;
	_matrix							_proj_matrix;
	int32_t							_max_content_length;
	std::wstring					_contents;
	ComPtr<ID3DXFont>				_font = nullptr;
	std::shared_ptr<Texture>		_texture_com = nullptr;
	std::shared_ptr<Shader>			_shader_com = nullptr;
	std::shared_ptr<ViBufferRect>	_vi_buffer_com = nullptr;

	TextBoxInformation				_text_info;
};

