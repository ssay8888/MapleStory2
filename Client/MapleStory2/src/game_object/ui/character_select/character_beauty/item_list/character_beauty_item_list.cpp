#include "c_pch.h"
#include "character_beauty_item_list.h"

#include "character_item.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterBeautyItemList::CharacterBeautyItemList() :
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT CharacterBeautyItemList::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterBeautyItemList::NativeConstruct(void* arg)
{
	_info = *static_cast<CreateItemListTextureInfo*>(arg);
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t CharacterBeautyItemList::Tick(const double timeDelta)
{
	for (auto& item : _items)
	{
		item->Tick(timeDelta);
	}
	return GameObject::Tick(timeDelta);
}

int32_t CharacterBeautyItemList::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT CharacterBeautyItemList::Render()
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

auto CharacterBeautyItemList::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyList"), TEXT("Com_Item_List"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture))))
		return E_FAIL;


	for (int i = 0; i < 6; ++i) 
	{
		CharacterItem::CreateItemInfo info;
		info.size = _float3(55.f, 55.f, 0.f);
		info.pos = _float3(_info.pos.x + (info.size.x / 2) - (_info.size.x / 2) + (57 * i),
			_info.pos.y + 40, 0);

		_items.push_back(CharacterItem::Create(&info));
	}
	return S_OK;
}