#include "c_pch.h"
#include "skill_level_change_btn.h"

#include "src/managers/sound_manager/sound_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

SkillLevelChangeBtn::SkillLevelChangeBtn(const std::function<void()> callback, const bool isMinus) :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_callback(callback),
	_is_minus(isMinus)
{
}

auto SkillLevelChangeBtn::NativeConstructPrototype() -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	return GameObject::NativeConstructPrototype();
}

auto SkillLevelChangeBtn::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto SkillLevelChangeBtn::Tick(double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto SkillLevelChangeBtn::Tick(_float3 pos, double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();
	_is_lbutton_down = _input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton);
	_is_lbutton_up = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton);
	ButtonStateChange(pos);
	return GameObject::Tick(timeDelta);
}

auto SkillLevelChangeBtn::LateTick(double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto SkillLevelChangeBtn::Render() -> HRESULT
{
	return GameObject::Render();
}

auto SkillLevelChangeBtn::Render(_float3 pos, std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 32;
	transformMatrix._22 = 32;
	if (_is_minus)
	{
		transformMatrix._41 = pos.x - 30;
		transformMatrix._42 = pos.y - 30;
	}
	else
	{
		transformMatrix._41 = pos.x + 30;
		transformMatrix._42 = pos.y - 30;
	}

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, _btn_state);

	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	return S_OK;
}

auto SkillLevelChangeBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto SkillLevelChangeBtn::Create(std::function<void()> callback, bool isMinus) -> std::shared_ptr<SkillLevelChangeBtn>
{
	auto pInstance = std::make_shared<SkillLevelChangeBtn>(callback, isMinus);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create SkillLevelChangeBtn");
		return nullptr;
	}
	return pInstance;
}

auto SkillLevelChangeBtn::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (_is_minus)
	{
		if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
			TEXT("Prototype_Texture_SkillUi_MinusBtn"),
			TEXT("Com_Texture"),
			reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
			TEXT("Prototype_Texture_SkillUi_PlusBtn"),
			TEXT("Com_Texture"),
			reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

auto SkillLevelChangeBtn::IsCollision(const _float3 pos) -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	if (_is_minus)
	{

		const RECT		rcUI = {
		   static_cast<LONG>((g_WinCX >> 1) + (pos.x - 30) - 32 * 0.5f),
		   static_cast<LONG>((g_WinCY >> 1) - (pos.y - 30) - 32 * 0.5f),
		   static_cast<LONG>((g_WinCX >> 1) + (pos.x - 30) + 32 * 0.5f),
		   static_cast<LONG>((g_WinCY >> 1) - (pos.y - 30) + 32 * 0.5f)
		};
		if (PtInRect(&rcUI, ptMouse))
		{
			return true;
		}
	}
	else
	{
		const RECT		rcUI = {
		   static_cast<LONG>((g_WinCX >> 1) + (pos.x + 30) - 32 * 0.5f),
		   static_cast<LONG>((g_WinCY >> 1) - (pos.y - 30) - 32 * 0.5f),
		   static_cast<LONG>((g_WinCX >> 1) + (pos.x + 30) + 32 * 0.5f),
		   static_cast<LONG>((g_WinCY >> 1) - (pos.y - 30) + 32 * 0.5f)
		};
		if (PtInRect(&rcUI, ptMouse))
		{
			return true;
		}
	}
	return false;
}

auto SkillLevelChangeBtn::ButtonStateChange(_float3 pos) -> void
{
	if (_btn_state == kPush)
	{
		if (!IsCollision(pos))
		{
			_btn_state = kEnable;
		}
		else
		{
			if (_is_lbutton_up)
			{
				_btn_state = kEnable;
				SoundManager::GetInstance().StopSound(SoundManager::kPotion);
				SoundManager::GetInstance().PlaySound(L"MBtMouseClick.mp3", SoundManager::kPotion);
			}
		}
		return;
	}
	else
	{
		_btn_state = kDisable;
	}

	if (0) //스킬포인트가있다면
	{
		_btn_state = kEnable;
	}

	if (IsCollision(pos))
	{
		_btn_state = kOver;
		if (_is_lbutton_down)
		{
			_btn_state = kPush;
		}
	}
}
