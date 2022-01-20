#include "c_pch.h"
#include "skill_set.h"

#include "protocol/game_protocol.pb.h"
#include "skilllevel_change_btn/skill_level_change_btn.h"
#include "src/game_object/ui/ket_set_ui/key_set_manager.h"
#include "src/game_object/ui/ket_set_ui/key_set_ui.h"
#include "src/game_object/ui/popup_info/popup_info.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/graphic/graphic_device.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_objects/manager/object_manager.h"

SkillSet::SkillSet(const int32_t skillId, const _float3 pos) :
	GameObject(GraphicDevice::GetInstance().GetDevice()),
	_skill_id(skillId),
	_pos(pos)
{
}

auto SkillSet::NativeConstructPrototype() -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	_size = { 65, 60, 0 };
	_input_device = InputDevice::Create(g_hInst, g_Wnd);
	_is_select_skill = false;
	return GameObject::NativeConstructPrototype();
}

auto SkillSet::NativeConstruct(void* arg) -> HRESULT
{
	return GameObject::NativeConstruct(arg);
}

auto SkillSet::Tick(const double timeDelta) -> HRESULT
{
	return GameObject::Tick(timeDelta);
}

auto SkillSet::Tick(const _float3 pos, const double timeDelta) -> HRESULT
{
	_input_device->InvalidateInputDevice();
	_is_lbutton_down = _input_device->GetDirectMouseKeyDown(InputDevice::kDirectInMouseButton::kLeftButton);
	_is_lbutton_up = _input_device->GetDirectMouseKeyUp(InputDevice::kDirectInMouseButton::kLeftButton);
	for (const auto& btn : _change_btn)
	{
		btn->Tick(_float3(
			pos.x + _pos.x - (g_WinCX >> 1),
			-pos.y + _pos.y + (g_WinCY >> 1), 0), timeDelta);
	}
	if (IsCollision(pos))
	{
		if (_is_lbutton_down)
		{
			_is_select_skill = true;
		}
	}
	if (_is_select_skill)
	{
		if (_is_lbutton_up)
		{
			_is_select_skill = false;
			auto baseKeyManager = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Keymanager", 0);
			auto keyManager = std::static_pointer_cast<KeySetManager>(baseKeyManager);
			if (keyManager)
			{
				auto index = keyManager->IsCollisionKeySet();
				if (index != -1)
				{
					const auto keySet = keyManager->GetKeySet(index);
					keySet->SetSkill(std::static_pointer_cast<SkillSet>(shared_from_this()));
					Protocol::GameClientKeySet sendPkt;
					sendPkt.set_key_value(index);
					sendPkt.set_type(Protocol::kSkill);
					sendPkt.set_value(_skill_id);
					SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
				}
			}
		}
	}
	_popup_info_com->Tick(timeDelta);
	return GameObject::Tick(timeDelta);
}

auto SkillSet::LateTick(const double timeDelta) -> HRESULT
{
	return GameObject::LateTick(timeDelta);
}

auto SkillSet::LateTick(_float3 pos, const double timeDelta) -> HRESULT
{
	for (const auto& btn : _change_btn)
	{
		btn->LateTick(timeDelta);
	}
	return GameObject::LateTick(timeDelta);
}


auto SkillSet::Render() -> HRESULT
{
	return GameObject::Render();
}

auto SkillSet::Render(_float3 pos, std::shared_ptr<Shader> shader) -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size.x;
	transformMatrix._22 = _size.y;
	transformMatrix._41 = pos.x + _pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -pos.y + _pos.y + (g_WinCY >> 1);

	auto result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _skillset_texture, 0);

	shader->Commit();
	_vi_buffer_com->RenderViBuffer();


	transformMatrix._11 = _size.x;
	transformMatrix._22 = _size.y;
	transformMatrix._41 = pos.x + _pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -pos.y + _pos.y + (g_WinCY >> 1);

	result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _skill_icon, 0);

	shader->Commit();
	_vi_buffer_com->RenderViBuffer();


	for (const auto& btn : _change_btn)
	{
		btn->Render(_float3(transformMatrix._41, transformMatrix._42, 0), shader);
	}

	if (_is_select_skill)
	{

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_Wnd, &ptMouse);

		const float centerX = g_WinCX >> 1;
		const float centerY = g_WinCY >> 1;
		transformMatrix._11 = _size.x;
		transformMatrix._22 = _size.y;
		transformMatrix._41 = (float)ptMouse.x - (g_WinCX >> 1);
		transformMatrix._42 = (float)-ptMouse.y + (g_WinCY >> 1);

		result = shader->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
		result = shader->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
		result = shader->SetUpTextureConstantTable("g_DiffuseTexture", _skill_icon, 0);

		shader->Commit();
		_vi_buffer_com->RenderViBuffer();

	}

	return S_OK;
}

auto SkillSet::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto SkillSet::Create(int32_t skillId, _float3 pos) -> std::shared_ptr<SkillSet>
{
	auto pInstance = std::make_shared<SkillSet>(skillId, pos);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create PlayerInfo");
		return nullptr;
	}
	return pInstance;
}

auto SkillSet::IsCollision(_float3 pos) -> bool
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_Wnd, &ptMouse);

	const RECT		rcUI = {
	   static_cast<LONG>((pos.x + _pos.x) - _size.x * 0.5f),
	   static_cast<LONG>((pos.y - _pos.y) - _size.y * 0.5f),
	   static_cast<LONG>((pos.x + _pos.x) + _size.x * 0.5f),
	   static_cast<LONG>((pos.y - _pos.y) + _size.y * 0.5f)
	};
	if (PtInRect(&rcUI, ptMouse))
	{
		return true;
	}
	return false;
}

auto SkillSet::GetSkillIcon() const -> std::shared_ptr<Texture>
{
	return _skill_icon;
}

auto SkillSet::GetSkillId() const -> int32_t
{
	return _skill_id;
}

auto SkillSet::GetPopupInfo() const -> std::shared_ptr<PopupInfo>
{
	return _popup_info_com;
}

auto SkillSet::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		TEXT("Prototype_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		TEXT("Prototype_Texture_SkillUi_SkillSet"),
		TEXT("Com_Texture"),
		reinterpret_cast<std::shared_ptr<Component>*>(&_skillset_texture))))
	{
		return E_FAIL;
	}

	const auto skillIconPrototype = fmt::format(L"Prototype_Texture_Skill_Icon_{}.png", _skill_id);
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0,
		fmt::format(L"Prototype_Texture_Skill_Icon_{}.png", _skill_id),
		fmt::format(L"Layer_Skillicon", _skill_id),
		reinterpret_cast<std::shared_ptr<Component>*>(&_skill_icon))))
	{
		return E_FAIL;
	}

	_change_btn[0] = SkillLevelChangeBtn::Create([]()
		{
			std::cout << "마이너스" << std::endl;
		}, true);

	_change_btn[1] = SkillLevelChangeBtn::Create([]()
		{
			std::cout << "플러스" << std::endl;
		}, false);



	_popup_info_com = PopupInfo::Create();
	_popup_info_com->SetItemTexture(_skill_icon);
	constexpr int skills[] = { 10200001, 10200011, 10200031, 10200041 };
	switch (_skill_id)
	{
	case 10200001:
		_popup_info_com->SetComment(L"라지소드를 휘둘러 암흑 피해를 입힌다.\n\n[상세 내용]\n명중 시 100%대미지\n스킬 연속 사용 시 반복 공격\n방향키 입력 시 이동 공격 가능");
		break;
	case 10200011:
		_popup_info_com->SetComment(L"라지소드와 함께 몸을 팽이처럼 회전시켜 암흑 피해를 입힌다.\n\n[상세 내용]\n명중 시 100% 대미지\n방향키 입력 시 이동 공격 가능");
		break;
	case 10200031:
		_popup_info_com->SetComment(L"어둠의 힘을 이용한다.\n\n[상세 내용]\n초당 SP 10 회복\n공격 명중 시 어둠의 기운 효과\n\n[추가 효과]\n어둠의 기운 : 중첩당 SP 재생력 1 증가, 초당 1회 발동, 최대 10 중첩");
		break;
	case 10200041:
		_popup_info_com->SetComment(L"라지소드를 하늘로 쳐올리며 돌격해 암흑 피해를 입힌다.\n\n[상세 내용]\n명중 시 100% 대미지\n스킬 연속 사용 시 반복 공격, 소모 SP 11로 감소\n반복 공격 명중 시 75% 대미지 2회\n슈퍼 아머 효과");
		break;
	}
	return S_OK;
}
