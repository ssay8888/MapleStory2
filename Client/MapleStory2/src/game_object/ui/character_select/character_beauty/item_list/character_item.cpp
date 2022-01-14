#include "c_pch.h"
#include "character_item.h"

#include "data_reader/data_reader_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterItem::CharacterItem() :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_state()
{
}

HRESULT CharacterItem::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterItem::NativeConstruct(void* arg)
{
	_info = *static_cast<CreateItemInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t CharacterItem::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t CharacterItem::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT CharacterItem::Render()
{
	return GameObject::Render();
}

auto CharacterItem::Render(std::shared_ptr<Shader> shader) const -> HRESULT
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
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture, _state);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();
	switch (_info.type)
	{
	case kEqpType::kPa:
	case kEqpType::kCl:
	case kEqpType::kSh:
		if (_texture_icon)
		{
			result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture_icon, 0);
			shader->Commit();
			_vi_buffer_com->RenderViBuffer();

		}
		break;
	case kEqpType::kFace:
		if (_texture_face_icon)
		{
			result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture_face_icon);
			shader->Commit();
			_vi_buffer_com->RenderViBuffer();
		}
		break;
	default: ;
	}
	return S_OK;
}

auto CharacterItem::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterItem::Create(CreateItemInfo* arg) -> std::shared_ptr<CharacterItem>
{
	auto instance = std::make_shared<CharacterItem>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}

auto CharacterItem::IsCollision() const -> bool
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

auto CharacterItem::GetButtonState() const -> kCreateItemState
{
	return _state;
}

auto CharacterItem::ChangeState(kCreateItemState state) -> void
{
	_state = state;
}

auto CharacterItem::GetManItemId()const ->int32_t
{
	return _info.man_item_id;
}

auto CharacterItem::GetGirlItemId()const ->int32_t
{
	return _info.girl_item_id;
}

auto CharacterItem::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyItem"), TEXT("Com_Item"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture))))
		return E_FAIL;

	switch (_info.type)
	{
	case kEqpType::kPa:
	case kEqpType::kCl:
	case kEqpType::kSh:
	{

		std::wstring prototypeName(L"Prototype_Texture_");
		if (_info.sex)
		{
			prototypeName.append(std::to_wstring(_info.girl_item_id));
		}
		else
		{
			prototypeName.append(std::to_wstring(_info.man_item_id));
		}
		GameObject::AddComponent(kScene::kSceneStatic, prototypeName, TEXT("Com_Item_Icon"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_icon));
		break;
	}
	case kEqpType::kFace:
	{
		int itemid = 0;
		if (_info.sex)
		{
			itemid = _info.girl_item_id;
		}
		else
		{
			itemid = _info.man_item_id;
		}
		const auto face = DataReaderManager::GetInstance().FindFace(itemid);
		_texture_face_icon = face->icon_diffuse_map;
		break;
	}
	default:;
	}

	return S_OK;
}
