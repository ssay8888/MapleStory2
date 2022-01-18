#include "c_pch.h"
#include "statup_btn.h"

#include "src/managers/character_stat/character_stat.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

StatupBtn::StatupBtn():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto StatupBtn::NativeConstructPrototype() -> HRESULT
{
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	return GameObject::NativeConstructPrototype();
}

auto StatupBtn::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto StatupBtn::Tick(double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto StatupBtn::LateTick(double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto StatupBtn::Render() -> HRESULT
{
	return GameObject::Render();
}

auto StatupBtn::Tick(_float3 pos, double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();
	if (CharacterStat::GetInstance().GetAp() <= 0)
	{
		_state = kDisable;
		return S_OK;
	}
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);
	const float centerX = g_WinCX >> 1;
	const float centerY = g_WinCY >> 1;

	RECT		rcUI = {
	   static_cast<LONG>((pos.x) - 18 * 0.5f),
	   static_cast<LONG>((pos.y) - 18 * 0.5f),
	   static_cast<LONG>((pos.x) + 18 * 0.5f),
	   static_cast<LONG>((pos.y) + 18 * 0.5f)
	};

	if (_state == kDown)
	{
		if (PtInRect(&rcUI, ptMouse))
		{
			if (_input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton))
			{
				_call();
			}
		}
		else
		{
			_state = kEnable;
		}
	}
	else
	{
		_state = kEnable;
		if (PtInRect(&rcUI, ptMouse))
		{
			_state = kOver;
			if (_input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton))
			{
				_state = kDown;
			}
		}
	}
	return S_OK;
}

auto StatupBtn::LateTick(_float3 pos, double timeDelta) -> HRESULT
{
	return S_OK;
}

auto StatupBtn::Render(_float3 pos, std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 18;
	transformMatrix._22 = 18;
	transformMatrix._41 = (pos.x) + -(g_WinCX >> 1);
	transformMatrix._42 = -(pos.y) + (g_WinCY >> 1);
	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _statup_btn, _state);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();
	return S_OK;
}

auto StatupBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<StatupBtn>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto StatupBtn::Create(std::function<void()> call) -> std::shared_ptr<StatupBtn>
{
	auto instance = std::make_shared<StatupBtn>();
	instance->_call = call;
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto StatupBtn::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_Statup_Btn"),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_statup_btn))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	return S_OK;
}
