#include "c_pch.h"
#include "key_set_ui.h"

#include "protocol/game_protocol.pb.h"
#include "src/game_object/item/Item.h"
#include "src/game_object/ui/skill_ui/skillset/skill_set.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

KeySetUi::KeySetUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

HRESULT KeySetUi::NativeConstructPrototype()
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstructPrototype();
}

HRESULT KeySetUi::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

auto KeySetUi::Tick(const double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();
	if (!g_isWindowsActive)
	{
		return 0;
	}
	if (_item || _skill)
	{
		if (IsCollision())
		{
			if (_input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kRightButton))
			{
				Protocol::GameClientKeySet sendPkt;
				sendPkt.set_key_value(_index);
				sendPkt.set_type(Protocol::kItem);
				sendPkt.set_value(-1);
				SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
				_item = nullptr;
				_skill = nullptr;
			}
		}

		if (_input_device->GetKeyDown(_key))
		{
			if (_item && _item->GetItemQuantity() > 0)
			{
				Protocol::GameClientItemApply sendPkt;
				sendPkt.set_type(Protocol::kInventoryEtc);
				sendPkt.set_position(_item->GetPosition());
				SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			}
			else if (_skill)
			{
				
			}
		}
	}


	return GameObject::Tick(timeDelta);
}

auto KeySetUi::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto KeySetUi::Render() -> HRESULT
{
	return GameObject::Render();
}

auto KeySetUi::Render(std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 42;
	transformMatrix._22 = 42;
	transformMatrix._41 = (_pos.x - (g_WinCX >> 1));
	transformMatrix._42 = (-_pos.y + (g_WinCY >> 1));
	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _free_set_ui, 0);
	shader->Commit();
	_vi_buffer_com->RenderViBuffer();

	if (_item)
	{
		auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _item->GetItemIcon(), 0);
		shader->Commit();
		_vi_buffer_com->RenderViBuffer();
	}
	else if (_skill)
	{
		auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _skill->GetSkillIcon(), 0);
		shader->Commit();
		_vi_buffer_com->RenderViBuffer();
	}
	return S_OK;
}

auto KeySetUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<KeySetUi>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto KeySetUi::SetItem(std::shared_ptr<Item> item) -> void
{
	_item = item;
	_skill = nullptr;
}

auto KeySetUi::SetSkill(std::shared_ptr<SkillSet> skill) -> void
{
	_item = nullptr;
	_skill = skill;
}

auto KeySetUi::IsCollision() -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>((_pos.x) - 42 * 0.5f),
	   static_cast<LONG>((_pos.y) - 42 * 0.5f),
	   static_cast<LONG>((_pos.x)  + 42 * 0.5f),
	   static_cast<LONG>((_pos.y) + 42 * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto KeySetUi::Create(const _float3 pos, const int32_t index, const int32_t key) -> std::shared_ptr<KeySetUi>
{
	auto instance = std::make_shared<KeySetUi>();
	instance->_pos = pos;
	instance->_index = index;
	instance->_key = key;
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto KeySetUi::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_KeySet"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_free_set_ui))))
	{
		return E_FAIL;
	}

	return S_OK;
}
