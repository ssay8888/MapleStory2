#include "c_pch.h"
#include "character_beauty_ui.h"

#include "character_beauty_select_sex.h"
#include "character_create_name_btn.h"
#include "character_create_name_popup.h"
#include "character_create_return_btn.h"
#include "data_reader/data_reader_manager.h"
#include "item_list/character_beauty_item_list.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

CharacterBeautyUi::CharacterBeautyUi(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

auto CharacterBeautyUi::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto CharacterBeautyUi::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	_info.size = _float3(360, 900, 0);
	_info.pos = _float3(g_WinCX - _info.size.x / 2, 0, 0);

	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

auto CharacterBeautyUi::Tick(const double timeDelta) -> HRESULT
{
	if (_beauty_stage != kBeautyStage::kEnd)
	{
		SexBtnTick(timeDelta);
		for (auto& list : _item_list)
		{
			list->Tick(timeDelta);
		}
		_return_btn->Tick(timeDelta);
		if (_create_name_btn->GetButtonState() == CharacterCreateNameBtn::kSelect)
		{
			_create_name_popup->ChangeShow(true);
		}
		_create_name_btn->Tick(timeDelta);
		_create_name_popup->Tick(timeDelta);
	}
	return GameObject::Tick(timeDelta);
}

auto CharacterBeautyUi::LateTick(const double timeDelta) -> HRESULT
{

	if (_beauty_stage != kBeautyStage::kEnd)
	{
		Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
		if (_return_btn->GetButtonState() == CharacterCreateReturnBtn::kSelect)
		{
			ChangeBeautyStage(kBeautyStage::kSelectInit);
		}
	}
	return GameObject::LateTick(timeDelta);
}

auto CharacterBeautyUi::Render() -> HRESULT
{

	if (_beauty_stage != kBeautyStage::kEnd)
	{
		_matrix			transformMatrix, viewMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		D3DXMatrixIdentity(&viewMatrix);
		transformMatrix._11 = _info.size.x;
		transformMatrix._22 = _info.size.y;
		transformMatrix._41 = _info.pos.x - (g_WinCX >> 1);
		transformMatrix._42 = -_info.pos.y;

		auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


		result = _shader_com->BeginShader(0);

		_vi_buffer_com->RenderViBuffer();

		for (auto i = 0; i < 2; ++i)
		{
			result = _sex_btn[i]->Render(_shader_com);
		}
		for (const auto& list : _item_list)
		{
			result = list->Render(_shader_com);
		}

		result = _return_btn->Render(_shader_com);
		result = _create_name_btn->Render(_shader_com);
		result = _create_name_popup->Render(_shader_com);
		result = _shader_com->EndShader();
	}
	return GameObject::Render();
}

auto CharacterBeautyUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<CharacterBeautyUi>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CharacterBeautyUi");
		return nullptr;
	}
	return pInstance;
}

auto CharacterBeautyUi::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CharacterBeautyUi>
{
	auto pInstance = std::make_shared<CharacterBeautyUi>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create CharacterBeautyUi");
		return nullptr;
	}
	return pInstance;
}

auto CharacterBeautyUi::GetBeautyStage() const -> kBeautyStage
{
	return _beauty_stage;
}

auto CharacterBeautyUi::ChangeBeautyStage(const kBeautyStage stage) -> void
{
	_beauty_stage = stage;
}

auto CharacterBeautyUi::GetSexState() const -> bool
{
	return _sex_select;
}

auto CharacterBeautyUi::AddComponents() -> HRESULT
{
	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_BeautyFrame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	if (FAILED(CreateSexButton()))
	{
		return E_FAIL;
	}

	if (FAILED(CreateItemList()))
	{
		return E_FAIL;
	}

	CharacterCreateReturnBtn::CreateNameBtnInfo info;
	info.size = _float3(31, 31, 0);
	info.pos = _float3(562, -250, 0);
	_return_btn = CharacterCreateReturnBtn::Create(&info);


	CharacterCreateNameBtn::CreateNameBtnInfo name_info;
	name_info.size = _float3(155, 50, 0);
	name_info.pos = _float3(460, -245, 0);
	_create_name_btn = CharacterCreateNameBtn::Create(&name_info);

	CharacterCreateNamePopup::CharacterCreateNamePopupInfo popup_info;
	popup_info.popup_size = _float3(245.f, 134.f, 0.f);
	popup_info.popup_pos = _float3(g_WinCX * 0.5f, g_WinCY * 0.5f, 0.f);
	popup_info.textbox_info.pos = { 54, 46 };
	popup_info.textbox_info.size = { 138, 16 };
	popup_info.textbox_info.text_color = D3DCOLOR_ARGB(255, 255, 255, 255);
	popup_info.textbox_info.font_info.Height = 11;
	popup_info.textbox_info.font_info.Width = 9;
	popup_info.textbox_info.font_info.Weight = FW_NORMAL;
	popup_info.sex = _sex_select;
	_create_name_popup = CharacterCreateNamePopup::Create(&popup_info);
	return S_OK;
}

auto CharacterBeautyUi::CreateSexButton() -> HRESULT
{
	for (auto i = 0; i < 2; ++i)
	{
		CharacterBeautySelectSex::CreateSexBtnInfo info;
		info.size = _float3(112.f, 62.f, 0.f);
		info.pos = _float3(1045 + (112.f * i), 65, 0.f);
		info.sex = i;
		_sex_btn[i] = CharacterBeautySelectSex::Create(&info);
	}
	return S_OK;
}

auto CharacterBeautyUi::CreateItemList() -> HRESULT
{
	_item_list.clear();
	const auto dataList = DataReaderManager::GetInstance().GetCreateItemInfo();
	int i = 0;
	for (auto& data : dataList)
	{
		CharacterBeautyItemList::CreateItemListTextureInfo info;
		info.title_name = data->title_name;
		info.size = _float3(340.f, 20.f, 0.f);
		info.pos = _float3(g_WinCX - 350.f / 2, 135.f + (80 * i), 0.f);
		for (auto& list : data->items)
		{
			if (list->gender)
			{
				info.girl_items.push_back(list->item_id);
			}
			else
			{
				info.man_items.push_back(list->item_id);
			}
			switch (list->type)
			{
			case 0:
				info.type = kEqpType::kCl;
				break;
			case 1:
				info.type = kEqpType::kPa;
				break;
			case 2:
				info.type = kEqpType::kFace;
				break;
			case 3:
				info.type = kEqpType::kSh;
				break;
			}
		}
		info.sex = _sex_select;
		_item_list.push_back(CharacterBeautyItemList::Create(&info));

		++i;
	}
	return S_OK;
}

auto CharacterBeautyUi::SexBtnTick(const double timeDelta) -> HRESULT
{
	int32_t select_index = -1;
	for (auto i = 0; i < 2; ++i)
	{
		_sex_btn[i]->Tick(timeDelta);
		if (_sex_btn[i]->IsCollision() && InputDevice::GetInstance().GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			select_index = i;
		}
	}
	if (select_index >= 0)
	{
		for (auto i = 0; i < 2; ++i)
		{
			if (i == select_index)
			{
				_sex_btn[i]->ChangeButtonState(CharacterBeautySelectSex::kCreateSexBtnState::kSelect);
				if (i == 0)
				{
					ChangeBeautyStage(kBeautyStage::kSexChangeMan);
					_sex_select = false;
					CreateItemList();
				}
				else
				{
					ChangeBeautyStage(kBeautyStage::kSexChangeGirl);
					_sex_select = true;
					CreateItemList();
				}
			}
			else
			{
				_sex_btn[i]->ChangeButtonState(CharacterBeautySelectSex::kCreateSexBtnState::kNormal);
			}
		}
	}
	return S_OK;
}

auto CharacterBeautyUi::GetCreateNameButton() const ->std::shared_ptr<CharacterCreateReturnBtn>
{
	return _return_btn;
}
