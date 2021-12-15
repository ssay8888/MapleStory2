#include "c_pch.h"
#include "character_create_return_btn.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include <src/system/input/input_device.h>

CharacterCreateReturnBtn::CharacterCreateReturnBtn() :
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT CharacterCreateReturnBtn::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterCreateReturnBtn::NativeConstruct(void* arg)
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

int32_t CharacterCreateReturnBtn::Tick(const double timeDelta)
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

int32_t CharacterCreateReturnBtn::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT CharacterCreateReturnBtn::Render()
{
	return GameObject::Render();
}

auto CharacterCreateReturnBtn::Render(const std::shared_ptr<Shader> shader) const -> HRESULT
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
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, 0);
		shader->Commit();
		_vi_buffer_com->RenderViBuffer();
	}
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, _state);
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();
	return S_OK;
}

auto CharacterCreateReturnBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterCreateReturnBtn::Create(CreateNameBtnInfo* arg) -> std::shared_ptr<CharacterCreateReturnBtn>
{
	auto instance = std::make_shared<CharacterCreateReturnBtn>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterCreateReturnBtn::GetButtonState() const -> kCreateBtnState
{
	return _state;
}

auto CharacterCreateReturnBtn::IsCollision() const -> bool
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

auto CharacterCreateReturnBtn::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_ReturnBtn"), TEXT("Com_RetrunBtn"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture))))
		return E_FAIL;
	
	return S_OK;
}
