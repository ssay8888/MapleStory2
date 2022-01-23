#include "c_pch.h"
#include "character_job_btn.h"

#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

#include <src/system/input/input_device.h>

#include "character_create_job_btn.h"

CharacterJobBtn::CharacterJobBtn() :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_state()
{
}

auto CharacterJobBtn::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterJobBtn::NativeConstruct(void* arg) -> HRESULT
{
	_info = *static_cast<CreateJobBtnInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}

	if(_info.job_id == 100)
	{
		_state = kSelect;
	}
	else
	{
		_state = kNormal;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterJobBtn::Tick(double timeDelta) -> HRESULT
{
	if (IsCollision())
	{
		if (_state == kNormal)
		{
			_state = kOver;
		}
	}
	else
	{
		if (_state == kOver)
		{
			_state = kNormal;
		}
	}
	_create_job_btn->Tick(timeDelta);
	return GameObject::Tick(timeDelta);
}

auto CharacterJobBtn::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto CharacterJobBtn::Render() -> HRESULT
{
	return GameObject::Render();
}

auto CharacterJobBtn::Render(const std::shared_ptr<Shader> shader) -> HRESULT
{
	switch(_state)
	{
	case kNormal:
		NormalRender(shader);
		break;
	case kOver:
		OverRender(shader);
		break;
	case kSelect:
		SelectRender(shader);
		break;
	case kEnd: 
		break;
	default: ;
	}

	return S_OK;
}

auto CharacterJobBtn::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterJobBtn::Create(CreateJobBtnInfo* arg) -> std::shared_ptr<CharacterJobBtn>
{
	auto instance = std::make_shared<CharacterJobBtn>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterJobBtn::IsCollision() const -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>(_info.pos.x - _info.size.x * 0.5f),
	   static_cast<LONG>(_info.pos.y - _info.size.y * 0.5f),
	   static_cast<LONG>(_info.pos.x + _info.size.x * 0.5f),
	   static_cast<LONG>(_info.pos.y + _info.size.y * 0.5f)
	};

	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto CharacterJobBtn::ChangeState(kCreateBtnState state) -> void
{
	_state = state;
}

auto CharacterJobBtn::GetCreateBtnState() const -> CharacterCreateJobBtn::kCreateJobBtnState
{
	return _create_job_btn->GetButtonState();
}

auto CharacterJobBtn::NormalRender(const std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _info.size.x;
	transformMatrix._22 = _info.size.y;
	transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_info.pos.y + (g_WinCY >> 1);

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture_front, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 1280;
	transformMatrix._22 = 500;
	transformMatrix._41 = 0;
	transformMatrix._42 = -75;

	return S_OK;
}

auto CharacterJobBtn::OverRender(const std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _info.size.x;
	transformMatrix._22 = _info.size.y;
	transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_info.pos.y + (g_WinCY >> 1);

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture_effect, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();
	
	return S_OK;
}

auto CharacterJobBtn::SelectRender(const std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _info.size.x;
	transformMatrix._22 = _info.size.y;
	transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_info.pos.y + (g_WinCY >> 1);

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture_effect, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 1280;
	transformMatrix._22 = 500;
	transformMatrix._41 = 0;
	transformMatrix._42 = -75;

	result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture_showinfo, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();



	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 600;
	transformMatrix._22 = 500;
	transformMatrix._41 = 250;
	transformMatrix._42 = -75;

	result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture_il, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();


	_create_job_btn->Render(shader);
	return S_OK;
}

auto CharacterJobBtn::AddComponents() -> HRESULT
{
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	std::wstring prototypeSelName(L"Prototype_Texture_");
	prototypeSelName.append(L"Sel_").append(_info.job_name).append(L"_il");

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, prototypeSelName, TEXT("Com_Select_Normal"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_Sel_FrontGround"), std::wstring(L"Com_Select_Over").append(L"_").append(_info.job_name), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture_front))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_SelectEffect"), std::wstring(L"Com_SelectEffect").append(L"_").append(_info.job_name), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture_effect))))
		return E_FAIL;

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_Sel_ShowInfo"), TEXT("Com_ShowInfo"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture_showinfo))))
		return E_FAIL;

	std::wstring prototypeName(L"Prototype_Texture_");
	prototypeName.append(_info.job_name).append(L"_il");

	if (FAILED(AddComponent(kScene::kSceneCharacterSelect, prototypeName, TEXT("Com_Job_Il"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture_il))))
		return E_FAIL;

	CharacterCreateJobBtn::CreateJobBtnInfo info;
	info.size = _float3(155, 50, 0);
	info.pos = _float3(g_WinCX >> 1, 630, 0);
	const auto instance = CharacterCreateJobBtn::Create(&info);
	_create_job_btn = instance;

	return S_OK;
}
