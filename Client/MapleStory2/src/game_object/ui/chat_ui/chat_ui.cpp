#include "c_pch.h"
#include "chat_ui.h"

#include "protocol/game_protocol.pb.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "string_utils/string_utils.h"

ChatUi::ChatUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto ChatUi::NativeConstructPrototype() -> HRESULT
{
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	ZeroMemory(&_fontInfo, sizeof(D3DXFONT_DESCW));
	_fontInfo.Height = 11;
	_fontInfo.Width = 11;
	_fontInfo.Weight = FW_HEAVY;
	_fontInfo.CharSet = HANGUL_CHARSET;
	lstrcpy(_fontInfo.FaceName, L"³Ø½¼Lv1°íµñ OTF");

	if (FAILED(D3DXCreateFontIndirectW(_graphic_device.Get(), &_fontInfo, &_font)))
	{
		MSGBOX("Creating Failed TextBoxUi");
		return E_FAIL;
	}
	return GameObject::NativeConstructPrototype();
}

auto ChatUi::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}

	EnableWindow(g_hEdit, true);
	_pos = { 188 , 680, 0 };
	return GameObject::NativeConstruct(arg);
}

auto ChatUi::Tick(const double timeDelta) -> HRESULT
{
	_rc = { static_cast<LONG>(_pos.x - (290 / 2)),
		static_cast<LONG>(_pos.y - (15 / 2)),
		static_cast<LONG>(_pos.x + (290 / 2)),
		static_cast<LONG>(_pos.y + (15 / 2)) };


	if (g_isWindowsActive && InputDevice::GetInstance().GetKeyDown(DIK_RETURN))
	{
		if (IsFocus())
		{
			Protocol::GameClientChat sendPkt;
			sendPkt.set_contents(StringUtils::ConvertWtoC(_contents.c_str()).c_str());
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			_contents.clear();
		}
		SetFocus(!IsFocus());
	}

	if (IsFocus())
	{
		wchar_t str[25];
		GetWindowText(g_hEdit, str, 25);
		SetText(str);
	}
	return GameObject::Tick(timeDelta);
}

auto ChatUi::LateTick(const double timeDelta) -> HRESULT
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto ChatUi::Render() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);

	_float3 pos { 188, 590, 0};
	transformMatrix._11 = 295;
	transformMatrix._22 = 150;
	transformMatrix._41 = (pos.x - (g_WinCX >> 1));
	transformMatrix._42 = -pos.y + (g_WinCY >> 1);
	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _frame_texture, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = 300;
	transformMatrix._22 = 30;
	//transformMatrix._41 = -450;
	//transformMatrix._42 = -320;
	transformMatrix._41 = (_pos.x - (g_WinCX >> 1));
	transformMatrix._42 = -_pos.y + (g_WinCY >> 1);

	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _chat_texture, _is_focus);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();


	result = _shader_com->EndShader();
	_font->DrawTextW(NULL, _contents.c_str(), -1, &_rc, DT_LEFT , D3DCOLOR_ARGB(255, 255, 255, 255));
	
	RECT rc = { static_cast<LONG>(pos.x - (280 / 2)),
		static_cast<LONG>(pos.y - (140 / 2)),
		static_cast<LONG>(pos.x + (280 / 2)),
		static_cast<LONG>(pos.y + (140 / 2)) };

	if (!_history.empty())
	{
		std::wstring str;
		if (_history.size() > 11)
		{
			for (auto i = _history.size() - 11; i < _history.size(); ++i)
			{
				if (i > 0)
				{
					str.append(_history[i]).append(L"\r\n");
				}
			}
		}
		else
		{
			for (auto i = 0; i < _history.size(); ++i)
			{
				str.append(_history[i]).append(L"\r\n");
			}
		}
		_font->DrawTextW(NULL, str.c_str(), -1, &rc, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	return GameObject::Render();
}

auto ChatUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<ChatUi>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto ChatUi::Create() -> std::shared_ptr<ChatUi>
{
	auto instance = std::make_shared<ChatUi>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto ChatUi::SetText(const std::wstring& str) -> void
{
	_contents = str;
}

auto ChatUi::GetText() const -> std::wstring
{
	return _contents;
}

auto ChatUi::IsFocus() const -> bool
{
	return _is_focus;
}

auto ChatUi::SetFocus(bool focus) -> void
{
	_is_focus = focus;
	g_isChatActive = _is_focus;
	if (_is_focus)
	{
		::SetFocus(g_hEdit);
		SetWindowText(g_hEdit, _contents.c_str());
		SendMessage(g_hEdit, EM_SETSEL, static_cast<WPARAM>(lstrlen(_contents.c_str())), static_cast<LPARAM>(lstrlen(_contents.c_str())));
	}
}

auto ChatUi::PushHistory(std::wstring content) -> void
{
	_history.push_back(content);
}

auto ChatUi::AddComponents() -> HRESULT
{
	if (FAILED(AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Chat_Frame"),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_frame_texture))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Chat"),
		TEXT("Com_Texture_1"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_chat_texture))))
	{
		return E_FAIL;
	}
	return S_OK;
}
