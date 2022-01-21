#pragma once
#include "src/utility/game_objects/game_object.h"

class Shader;
class ViBufferRect;
class Texture;

class ChatUi : public GameObject
{
public:
	ChatUi();
	~ChatUi() override = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> HRESULT override;
	auto LateTick(double timeDelta) -> HRESULT override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

	static auto Create() -> std::shared_ptr<ChatUi>;

public:
	auto SetText(const std::wstring& str)->void;
	auto GetText() const->std::wstring;
	auto IsFocus() const ->bool;
	auto SetFocus(bool focus)->void;

	auto PushHistory(std::wstring content)->void;


private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Texture>							_frame_texture = nullptr;
	std::shared_ptr<Texture>							_chat_texture = nullptr;
	std::shared_ptr<ViBufferRect>						_vi_buffer_com = nullptr;
	std::shared_ptr<Shader>								_shader_com = nullptr;



	RECT							_rc;
	bool							_is_focus = false;
	std::wstring					_contents;
	std::vector<std::wstring>							_history;
	ComPtr<ID3DXFont>				_font = nullptr;

	_float3 _pos;
	D3DXFONT_DESCW _fontInfo;
	_matrix		_proj_matrix;
};

