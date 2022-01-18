#include "c_pch.h"
#include "character_beauty_item_list.h"

#include "character_item.h"
#include "src/game_object/fittingdoll/fittingdoll.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"

CharacterBeautyItemList::CharacterBeautyItemList() :
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto CharacterBeautyItemList::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterBeautyItemList::NativeConstruct(void* arg) -> HRESULT
{
	_info = *static_cast<CreateItemListTextureInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterBeautyItemList::Tick(const double timeDelta) -> HRESULT
{
	auto& inputDevice = InputDevice::GetInstance();

	bool select = false;
	const size_t size = _items.size();
	for (size_t i = 0; i < size; ++i)
	{
		const auto item = _items[i];
		if (item->IsCollision() && inputDevice.GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			select = true;
			_select_index = static_cast<int32_t>(i);
			break;
		}
	}

	if (select)
	{
		for (size_t i = 0; i < size; ++i)
		{
			const auto item = _items[i];
			if (i == _select_index)
			{
				item->ChangeState(CharacterItem::kSelect);
				auto playerObject = std::static_pointer_cast<Fittingdoll>(
					ObjectManager::GetInstance().GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
				if (_info.sex)
				{
					playerObject->ChangeEqp(GameContents::EquipeType(item->GetGirlItemId()), item->GetGirlItemId());
				}
				else
				{
					playerObject->ChangeEqp(GameContents::EquipeType(item->GetManItemId()), item->GetManItemId());
				}
			}
			else
			{
				item->ChangeState(CharacterItem::kNormal);
			}
		}
	}
	for (const auto& item : _items)
	{
		item->Tick(timeDelta);
	}
	return GameObject::Tick(timeDelta);
}

auto CharacterBeautyItemList::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto CharacterBeautyItemList::Render() -> HRESULT
{
	return GameObject::Render();
}

auto CharacterBeautyItemList::Render(std::shared_ptr<Shader> shader) const -> HRESULT
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
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _texture, 0);
	shader->Commit();

	_vi_buffer_com->RenderViBuffer();

	RECT rc
	{
		static_cast<LONG>(_info.pos.x - _info.size.x / 2) + 4,
		static_cast<LONG>(_info.pos.y- _info.size.y / 2) + 4,
		static_cast<LONG>(_info.pos.x + _info.size.x / 2),
		static_cast<LONG>(_info.pos.y + _info.size.y / 2)
	};

	GraphicDevice::GetInstance().GetFont()->DrawTextW(NULL, _info.title_name.c_str(), -1, &rc, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));

	for (auto& item : _items)
	{
		item->Render(shader);
	}
	return S_OK;
}

auto CharacterBeautyItemList::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto CharacterBeautyItemList::Create(CreateItemListTextureInfo* arg) -> std::shared_ptr<CharacterBeautyItemList>
{
	auto instance = std::make_shared<CharacterBeautyItemList>();

	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}

	return instance;
}
auto CharacterBeautyItemList::InitEqp() -> void
{
	auto playerObject = std::static_pointer_cast<Fittingdoll>(
		ObjectManager::GetInstance().GetGameObjectPtr(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"), 0));
	const auto item = _items[0];
	if (item)
	{
		if (_info.sex)
		{
			playerObject->ChangeEqp(GameContents::EquipeType(item->GetGirlItemId()), item->GetGirlItemId());
		}
		else
		{
			playerObject->ChangeEqp(GameContents::EquipeType(item->GetManItemId()), item->GetManItemId());
		}
		item->ChangeState(CharacterItem::kSelect);
	}
}


auto CharacterBeautyItemList::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyList"), TEXT("Com_Item_List"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture))))
		return E_FAIL;


	for (size_t i = 0; i < 6; ++i) 
	{
		CharacterItem::CreateItemInfo info;
		info.size = _float3(55.f, 55.f, 0.f);
		info.pos = _float3(_info.pos.x + (info.size.x / 2) - (_info.size.x / 2) + (57 * i),
			_info.pos.y + 40, 0);

		bool isItem = false;
		if (!_info.man_items.empty() && _info.man_items.size() > i)
		{
			info.man_item_id = _info.man_items[i];
			isItem = true;
		}

		if (!_info.girl_items.empty() && _info.girl_items.size() > i)
		{
			info.girl_item_id = _info.girl_items[i];
			isItem = true;
		}
		info.sex = _info.sex;
		info.type = _info.type;
		if (isItem)
		{
			_items.push_back(CharacterItem::Create(&info));
		}
	}
	return S_OK;
}
