#include "c_pch.h"
#include "skill_ui.h"

#include "skillset/skill_set.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/managers/sound_manager/sound_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

SkillUi::SkillUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
	_moving = false;
	_is_show = false;
}

auto SkillUi::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto SkillUi::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return S_OK;
	}
	_size.x = 486;
	_size.y = 169;
	_pos.x = g_WinCX >> 1;
	_pos.y = g_WinCY >> 1;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto SkillUi::Tick(const double timeDelta) -> HRESULT
{
	if (!_is_show)
	{
		return S_OK;
	}
	_input_device->InvalidateInputDevice();
	_is_lbutton_down = _input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton);
	_is_lbutton_up = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton);
	for (const auto& skillset : _skill_sets)
	{
		skillset.second->Tick(_pos, timeDelta);
	}
	CloseButtonTick(timeDelta);
	if (FAILED(MoveInventoryFrame()))
	{
		return E_FAIL;
	}
	return GameObject::Tick(timeDelta);
}

auto SkillUi::LateTick(const double timeDelta) -> HRESULT
{
	if (!_is_show)
	{
		return S_OK;
	}
	for (const auto& skillset : _skill_sets)
	{
		skillset.second->LateTick(_pos, timeDelta);
	}
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto SkillUi::Render() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size.x;
	transformMatrix._22 = _size.y;
	transformMatrix._41 = _pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_pos.y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();

	transformMatrix._11 = 24;
	transformMatrix._22 = 24;
	transformMatrix._41 = (_pos.x + 220) + -(g_WinCX >> 1);
	transformMatrix._42 = -(_pos.y - 58) + (g_WinCY >> 1);
	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _inventory_close_btn, _close_btn_over);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	for (const auto& skillset : _skill_sets)
	{
		skillset.second->Render(_pos, _shader_com);
	}
	result = _shader_com->EndShader();

	for (const auto& skillset : _skill_sets)
	{
		if (skillset.second->IsCollision(_pos))
		{
			skillset.second->GetPopupInfo()->Render();
		}
	}
	return GameObject::Render();
}

auto SkillUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<SkillUi>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone PlayerInfo");
		return nullptr;
	}
	return pInstance;
}

auto SkillUi::ChangeShow() -> void
{
	_is_show = !_is_show;
	_pos = { g_WinCX / 2.f, g_WinCY / 2.f, 0 };
	SoundManager::GetInstance().StopSound(SoundManager::kUi);
	if (_is_show)
	{
		SoundManager::GetInstance().PlaySound(L"System_defaultDialog_open_02.wav", SoundManager::kUi);
	}
	else
	{
		SoundManager::GetInstance().PlaySound(L"System_defaultDialog_close_02.wav", SoundManager::kUi);
	}
}

auto SkillUi::GetSkillSet(const int32_t skillId) -> std::shared_ptr<SkillSet>
{
	return _skill_sets[skillId];
}

auto SkillUi::Create() -> std::shared_ptr<SkillUi>
{
	auto pInstance = std::make_shared<SkillUi>();

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create PlayerInfo");
		return nullptr;
	}
	return pInstance;
}

auto SkillUi::AddComponents() -> HRESULT
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, 
		TEXT("Prototype_VIBuffer_Rect"), 
		TEXT("Com_VIBuffer"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, 
		TEXT("Prototype_Texture_SkillUi"), 
		TEXT("Com_Texture"), 
		reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Inventory_Close_Btn"),
		TEXT("Com_Texture_2"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_inventory_close_btn))))
		return E_FAIL;

	constexpr int skills[] = { 10200001, 10200011, 10200031, 10200041 };
	int i = 0;
	for (auto skill : skills)
	{
		_skill_sets[skill] = SkillSet::Create(skill, _float3(-150.f + (i * 100.f),-25.f,0));
		i++;
	}

	return S_OK;
}

auto SkillUi::CloseButtonTick(double timeDelta) -> HRESULT
{
	_close_btn_over = false;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>((_pos.x + 220) - 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 58) - 24 * 0.5f),
	   static_cast<LONG>((_pos.x + 220) + 24 * 0.5f),
	   static_cast<LONG>((_pos.y - 58) + 24 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		_close_btn_over = true;
		if (_is_lbutton_down)
		{
			_is_show = !_is_show;
		}
	}
	return S_OK;
}

auto SkillUi::MoveInventoryFrame() -> HRESULT
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>(_pos.x - _size.x * 0.5f),
	   static_cast<LONG>(_pos.y - _size.y * 0.5f),
	   static_cast<LONG>(_pos.x + _size.x * 0.5f),
	   static_cast<LONG>(_pos.y - ((_size.y * 0.5f) - 55))
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		if (_is_lbutton_down)
		{
			_moving = true;
		}
	}
	if (_moving)
	{
		if (_is_lbutton_up)
		{
			_moving = false;
		}

		auto x = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kX);
		auto y = InputDevice::GetInstance().GetDirectMouseMoveState(InputDevice::kDirectInMouseState::kY);

		_pos.x += x;
		_pos.y += y;
	}
	return S_OK;
}
