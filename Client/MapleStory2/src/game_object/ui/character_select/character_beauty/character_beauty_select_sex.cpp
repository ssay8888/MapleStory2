#include "c_pch.h"
#include "character_beauty_select_sex.h"

#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterBeautySelectSex::CharacterBeautySelectSex() :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_state(kNormal)
{
}

auto CharacterBeautySelectSex::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterBeautySelectSex::NativeConstruct(void* arg) -> HRESULT
{

	_info = *static_cast<CreateSexBtnInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterBeautySelectSex::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto CharacterBeautySelectSex::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto CharacterBeautySelectSex::Render() -> HRESULT
{
	return GameObject::Render();
}

auto CharacterBeautySelectSex::Render(std::shared_ptr<Shader> shader) const -> HRESULT
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
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _select_texture, _state);
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();

	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 28;
	transformMatrix._22 = 45;
	transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_info.pos.y + (g_WinCY >> 1);

	result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	if (_state == kOver || _state == kNormal)
	{
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _sex_texture, 0);
	}
	else if (_state == kSelect)
	{
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _sex_texture, 1);
	}
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();
	
	return S_OK;
}

auto CharacterBeautySelectSex::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterBeautySelectSex::Create(CreateSexBtnInfo* arg) -> std::shared_ptr<CharacterBeautySelectSex>
{
	auto instance = std::make_shared<CharacterBeautySelectSex>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterBeautySelectSex::IsCollision() const -> bool
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

auto CharacterBeautySelectSex::GetButtonState() const -> kCreateSexBtnState
{
	return _state;
}

auto CharacterBeautySelectSex::ChangeButtonState(const kCreateSexBtnState state) -> void
{
	_state = state;
}

auto CharacterBeautySelectSex::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (_info.sex)
	{
		if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexGirlBtn"), TEXT("Com_Select_SexBtn"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture))))
			return E_FAIL;
		if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexGirlImage"), TEXT("Com_Select_SexImage"), reinterpret_cast<std::shared_ptr<Component>*>(&_sex_texture))))
			return E_FAIL;
		
	}
	else
	{
		if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexManBtn"), TEXT("Com_Select_SexBtn"), reinterpret_cast<std::shared_ptr<Component>*>(&_select_texture))))
			return E_FAIL;
		if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SexManImage"), TEXT("Com_Select_SexImage"), reinterpret_cast<std::shared_ptr<Component>*>(&_sex_texture))))
			return E_FAIL;
		_state = kSelect;
	}
	return S_OK;
}
