#include "c_pch.h"
#include "character_create_name_btn.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include <src/system/input/input_device.h>

CharacterCreateNameBtn::CharacterCreateNameBtn() :
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto CharacterCreateNameBtn::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterCreateNameBtn::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}

	_info = *static_cast<CreateNameBtnInfo*>(arg);
	_state = kNormal;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterCreateNameBtn::Tick(const double timeDelta) -> HRESULT
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

auto CharacterCreateNameBtn::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto CharacterCreateNameBtn::Render() -> HRESULT
{
	return GameObject::Render();
}

auto CharacterCreateNameBtn::Render(const std::shared_ptr<Shader> shader) const -> HRESULT
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
	if (_state == kOver)
	{
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture[_state], 0);
		shader->Commit();

		_vi_buffer_com->RenderViBuffer();
	}
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture[_state], 0);
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();
	return S_OK;
}

auto CharacterCreateNameBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterCreateNameBtn::Create(CreateNameBtnInfo* arg) -> std::shared_ptr<CharacterCreateNameBtn>
{
	auto instance = std::make_shared<CharacterCreateNameBtn>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterCreateNameBtn::IsCollision() const -> bool
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

auto CharacterCreateNameBtn::GetButtonState() const -> kCreateJobBtnState
{
	return _state;
}

auto CharacterCreateNameBtn::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Normal"), TEXT("Com_Select_Normal"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kNormal]))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Over"), TEXT("Com_Select_Over"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kOver]))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectItem_Select"), TEXT("Com_Select_Select"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture[kSelect]))))
		return E_FAIL;


	return S_OK;
}
