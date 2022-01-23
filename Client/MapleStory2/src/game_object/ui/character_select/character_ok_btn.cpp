#include "c_pch.h"
#include "character_ok_btn.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include <src/system/input/input_device.h>

#include "src/managers/characters_manager/character.h"
#include "src/managers/characters_manager/characters_manager.h"
#include "src/network/login_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/utility/timers/timer_manager.h"

CharacterOkBtn::CharacterOkBtn() :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_state()
{
}

auto CharacterOkBtn::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterOkBtn::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}

	_info = *static_cast<OkBtnInfo*>(arg);
	_state = kNormal;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterOkBtn::Tick(const double timeDelta) -> HRESULT
{
	if (IsCollision())
	{
		if (InputDevice::GetInstance().GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			_state = kSelect;
		}
		else
		{
			_state = kOver;
		}
	}
	else
	{
		_state = kNormal;
	}
	return GameObject::Tick(timeDelta);
}

auto CharacterOkBtn::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto CharacterOkBtn::Render() -> HRESULT
{
	return GameObject::Render();
}

auto CharacterOkBtn::Render(const std::shared_ptr<Shader> shader) const -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _info.size.x;
	transformMatrix._22 = _info.size.y;
	transformMatrix._41 = _info.pos.x;
	transformMatrix._42 = _info.pos.y;

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture[_state], 0);
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();
	DrawTextContents();

	return S_OK;
}

auto CharacterOkBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterOkBtn::Create(OkBtnInfo* arg) -> std::shared_ptr<CharacterOkBtn>
{
	auto instance = std::make_shared<CharacterOkBtn>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterOkBtn::IsCollision() const -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	int32_t centerX = g_WinCX >> 1;
	int32_t centerY = g_WinCY >> 1;
	const RECT		rcUI = {
	   static_cast<LONG>(centerX + (_info.pos.x - _info.size.x * 0.5f)),
	   static_cast<LONG>(centerY + (-_info.pos.y - _info.size.y * 0.5f)),
	   static_cast<LONG>(centerX + (_info.pos.x + _info.size.x * 0.5f)),
	   static_cast<LONG>(centerY + (-_info.pos.y + _info.size.y * 0.5f))
	};

	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto CharacterOkBtn::SelectCharacter(int32_t index) const -> bool
{
	auto& timerManager = TimerManager::GetInstance();
	constexpr float limitTime = 1.f;
	if (timerManager.IsTimeCheck(TEXT("OkBtnDelay"), limitTime))
	{
		auto& instance = CharactersManager::GetInstance();
		Protocol::LoginClientCharacterSelect sendPkt;
		if (auto character = instance.GetCharacter(index))
		{
			sendPkt.set_characterid(character->GetInfo().characterid());
		}
		SendManager::GetInstance().Push(LoginServerPacketHandler::MakeSendBuffer(sendPkt));
		timerManager.ResetTime(TEXT("OkBtnDelay"));
		return true;
	}
	return false;
}

auto CharacterOkBtn::AddComponents() -> HRESULT
{
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Normal"), TEXT("Com_Select_Normal"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kNormal]))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Over"), TEXT("Com_Select_Over"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kOver]))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Select"), TEXT("Com_Select_Select"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kSelect]))))
		return E_FAIL;

	auto& timerManager = TimerManager::GetInstance();
	timerManager.AddTimers(TEXT("OkBtnDelay"));

	return S_OK;
}

auto CharacterOkBtn::DrawTextContents() const -> void
{
	int32_t centerX = g_WinCX >> 1;
	int32_t centerY = g_WinCY >> 1;
	RECT rc
	{
	   static_cast<LONG>(centerX + (_info.pos.x - _info.size.x * 0.5f)),
	   static_cast<LONG>(centerY + (-_info.pos.y - _info.size.y * 0.5f)),
	   static_cast<LONG>(centerX + (_info.pos.x + _info.size.x * 0.5f)),
	   static_cast<LONG>(centerY + (-_info.pos.y + _info.size.y * 0.5f))
	};

	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, L"게임시작", -1, &rc, DT_VCENTER | DT_CENTER, D3DCOLOR_ARGB(255, 0, 0, 0));

}