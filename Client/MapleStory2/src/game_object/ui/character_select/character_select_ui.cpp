#include "c_pch.h"
#include "character_select_ui.h"

#include "character_create_btn.h"
#include "character_select_item.h"
#include "character_create/character_job_btn.h"
#include "src/managers/characters_manager/characters_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include <src/game_object/fittingdoll/fittingdoll.h>

#include "character_ok_btn.h"
#include "src/managers/characters_manager/character.h"

CharacterSelectUi::CharacterSelectUi(const ComPtr<IDirect3DDevice9>& device) :
	GameObject(device),
	_x(0),
	_y(0),
	_size_x(0),
	_size_y(0)
{
}

HRESULT CharacterSelectUi::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT CharacterSelectUi::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	if (FAILED(CreateSelectList()))
		return E_FAIL;

	if (FAILED(CreateCharacterBtn()))
		return E_FAIL;

	if (FAILED(CreateJobBtnList()))
		return E_FAIL;
	
	_character_list.reserve(_max_character_slot);
	_size_x = 461 - 100;
	_size_y = 720;
	_x = g_WinCX - _size_x / 2;
	_y = g_WinCY - _size_y * 0.5f;

	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t CharacterSelectUi::Tick(double timeDelta)
{
	switch (_state)
	{
	case kCharacterSelectState::kSelect:
		SelectUpdateTick(timeDelta);
		break;
	case kCharacterSelectState::kCreateJob:
		CreateJobUpdateTick(timeDelta);
		break;
	case kCharacterSelectState::kBeauty: 
		break;
	default: ;
	}
	return GameObject::Tick(timeDelta);
}

int32_t CharacterSelectUi::LateTick(const double timeDelta)
{
	GameObject::Render();

	for (const auto& item : _character_list)
	{
		item->LateTick(timeDelta);
	}
	_create_btn->LateTick(timeDelta);
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());

	return S_OK;
}

HRESULT CharacterSelectUi::Render()
{
	switch (_state)
	{
	case kCharacterSelectState::kSelect:
		SelectRender();
		break;
	case kCharacterSelectState::kCreateJob:
		SelectJobRender();
		break;
	case kCharacterSelectState::kBeauty:
		break;
	default:;
	}
	return GameObject::Render();
}

auto CharacterSelectUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<CharacterSelectUi>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone UI");
		return nullptr;
	}
	return pInstance;
}

auto CharacterSelectUi::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<CharacterSelectUi>
{
	auto pInstance = std::make_shared<CharacterSelectUi>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create UI");
		return nullptr;
	}
	return pInstance;
}

auto CharacterSelectUi::GetState() const -> kCharacterSelectState
{
	return _state;
}

auto CharacterSelectUi::ChangeState(kCharacterSelectState state) -> void
{
	_state = state;
}

auto CharacterSelectUi::SelectUpdateTick(const double timeDelta) -> HRESULT
{
	auto& instance = InputDevice::GetInstance();

	bool select = false;
	const size_t size = _character_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		const auto item = _character_list[i];
		if (item->IsCollision() && instance.GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			select = true;
			_select_item_index = static_cast<int32_t>(i);
			break;
		}
	}
	if (select)
	{
		for (size_t i = 0; i < size; ++i)
		{
			const auto item = _character_list[i];
			if (i == _select_item_index)
			{
				item->ChangeState(CharacterSelectItem::kSelect);
				CreateFittingDoll(static_cast<int32_t>(i));
			}
			else
			{
				item->ChangeState(CharacterSelectItem::kNormal);
			}
		}
	}

	for (size_t i = 0; i < size; ++i)
	{
		const auto item = _character_list[i];
		item->Tick(timeDelta);
	}
	if (_create_btn)
	{
		_create_btn->Tick(timeDelta);
		if (_create_btn->IsCollision() && instance.GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			ChangeState(kCharacterSelectState::kCreateJob);
			_create_btn->Tick(timeDelta);
		}
	}
	if (_ok_btn)
	{
		_ok_btn->Tick(timeDelta);
		if (_ok_btn->IsCollision() && instance.GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			auto result = _ok_btn->SelectCharacter(_select_item_index);
			_ok_btn->Tick(timeDelta);
		}
	}
	return S_OK;
}

auto CharacterSelectUi::CreateJobUpdateTick(double timeDelta) -> HRESULT
{
	auto& instance = InputDevice::GetInstance();

	bool select = false;


	const size_t size = _character_job_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		const auto item = _character_job_list[i];
		if (item->IsCollision() && instance.GetDirectMouseKeyPressing(InputDevice::kDirectInMouseButton::kLeftButton))
		{
			select = true;
			_select_job_index = static_cast<int32_t>(i);
			break;
		}
	}
	if (select)
	{
		for (size_t i = 0; i < size; ++i)
		{
			const auto item = _character_job_list[i];
			if (i == _select_job_index)
			{
				item->ChangeState(CharacterJobBtn::kSelect);
			}
			else
			{
				item->ChangeState(CharacterJobBtn::kNormal);
			}
		}
	}
	for (const auto& item : _character_job_list)
	{
		item->Tick(timeDelta);
		if (item->GetCreateBtnState() == CharacterCreateJobBtn::kCreateJobBtnState::kSelect)
		{
			ChangeState(kCharacterSelectState::kBeautyInit);
			item->ChangeState(CharacterJobBtn::kNormal);
		}
	}

	return S_OK;
}

auto CharacterSelectUi::SelectRender() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -_y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	for (const auto& item : _character_list)
	{
		item->Render(_shader_com);
	}
	if (_create_btn)
	{
		result = _create_btn->Render(_shader_com);
	}
	if (_ok_btn)
	{
		result = _ok_btn->Render(_shader_com);
	}

	result = _shader_com->EndShader();
	return S_OK;
}

auto CharacterSelectUi::SelectJobRender() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -_y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	for (const auto& item : _character_list)
	{
		item->Render(_shader_com);
	}

	if (_create_btn)
	{
		result = _create_btn->Render(_shader_com);
	}

	if (_ok_btn)
	{
		result = _ok_btn->Render(_shader_com);
	}


	// 백그라운드(뒤에 까만거)
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 1280;
	transformMatrix._22 = 720;
	transformMatrix._41 = 0;
	transformMatrix._42 = 0;

	result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_background, 0);
	_shader_com->Commit();
	_vi_buffer_com->RenderViBuffer();

	for (const auto& item : _character_job_list)
	{
		item->Render(_shader_com);
	}
	result = _shader_com->EndShader();
	return S_OK;
}

auto CharacterSelectUi::AddComponents() -> HRESULT
{
	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_SelectFrame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Rect",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;


	if (FAILED(GameObject::AddComponent(kScene::kSceneCharacterSelect, TEXT("Prototype_Texture_Create_BackGround"), TEXT("Com_Texture_BackGround"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_background))))
		return E_FAIL;
	
	return S_OK;
}

auto CharacterSelectUi::CreateSelectList() -> HRESULT
{
	for (int i = 0; i < _max_character_slot; ++i)
	{
		CharacterSelectItem::SelectItemInfo info;
		info.size = _float3(330, 64, 0);
		info.pos = _float3(1100, (160.f + (68.f * i)), 0);
		info.index = i;
		auto instance = CharacterSelectItem::Create(&info);
		_character_list.push_back(instance);
	}
	return S_OK;
}

auto CharacterSelectUi::CreateCharacterBtn() -> HRESULT
{
	CharacterCreateBtn::CreateBtnInfo info;
	info.size = _float3(155, 50, 0);
	info.pos = _float3(460, -270, 0);
	const auto instance = CharacterCreateBtn::Create(&info);
	_create_btn = instance;
	_ok_btn = nullptr;
	return S_OK;
}

auto CharacterSelectUi::CreateOkBtn() -> HRESULT
{
	CharacterOkBtn::OkBtnInfo info;
	info.size = _float3(155, 50, 0);
	info.pos = _float3(460, -270, 0);
	const auto instance = CharacterOkBtn::Create(&info);
	_ok_btn = instance;
	_create_btn = nullptr;
	return S_OK;
}

auto CharacterSelectUi::CreateJobBtnList() -> HRESULT
{
	std::pair<int32_t, std::wstring> jobs[]{ {100, L"Night"}, {300, L"Wizard"}, {500, L"Ranger"}, {700,  L"Thief"}, {900, L"RuneBlader"},
		{1100, L"SoulBinder"}, {200, L"Berserker"}, {400, L"Priest"} , {600,  L"HeavyGunner"}, {800, L"Assassin"}, {1000,  L"Striker"} };

	int32_t i = 0;
	const float x_size = 123.f;
	const float y_size = 76.f;
	for (const auto& job : jobs)
	{
		CharacterJobBtn::CreateJobBtnInfo info;
		info.size = _float3(x_size, y_size, 0);
		if (i < 6)
		{
			info.pos = _float3(x_size + (x_size * i++) + (i * 3), (y_size / 2) + 10, 0);
		}
		else
		{
			info.pos = _float3(x_size + (x_size * (i++ % 6)) + ((i % 6) * 3), y_size * 2 - (y_size / 2) + 10 + 3, 0);
		}
		info.job_id = job.first;
		info.job_name = job.second;

		const auto instance = CharacterJobBtn::Create(&info);
		_character_job_list.push_back(instance);
	}
	return S_OK;
}

auto CharacterSelectUi::CreateFittingDoll(int32_t index) -> HRESULT
{
	auto& instance = ObjectManager::GetInstance();
	instance.LayerClear(kSceneCharacterSelect, TEXT("Layer_Fittingdoll"));
	auto& characterManager = CharactersManager::GetInstance();
	auto character = characterManager.GetCharacter(index);
	if (character)
	{
		Fittingdoll::FittingdollInfo info;
		info.sex = character->GetInfo().gender();
		info.character = character;
		if (FAILED(instance.AddGameObject(kSceneCharacterSelect, TEXT("Prototype_Mesh_Fittingdool"), TEXT("Layer_Fittingdoll"), &info)))
		{
			return E_FAIL;
		}
		CreateOkBtn();
	}
	else
	{
		CreateCharacterBtn();
		
	}
	return S_OK;
}
