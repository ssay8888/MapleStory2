#include "c_pch.h"
#include "key_set_manager.h"

#include "key_set_ui.h"
#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/inventory/inventory_tab_btn/inventory_tab_btn.h"
#include "src/game_object/ui/skill_ui/skill_ui.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/game_objects/manager/object_manager.h"

KeySetManager::KeySetManager(): GameObject(GraphicDevice::GetInstance().GetDevice())
{
}

auto KeySetManager::NativeConstructPrototype() -> HRESULT
{
	return GameObject::NativeConstructPrototype();
}

auto KeySetManager::NativeConstruct(void* arg) -> HRESULT
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	return GameObject::NativeConstruct(arg);
}

auto KeySetManager::Tick(const double timeDelta) -> HRESULT
{
	for (const auto& key : _key_sets)
	{
		key->Tick(timeDelta);
	}
	return GameObject::Tick(timeDelta);
}

auto KeySetManager::LateTick(const double timeDelta) -> HRESULT
{
	for (const auto& key : _key_sets)
	{
		key->LateTick(timeDelta);
	}

	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

auto KeySetManager::Render() -> HRESULT
{
	auto result = _shader_com->BeginShader(0);

	for (const auto& key : _key_sets)
	{
		key->Render(_shader_com);
	}

	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto KeySetManager::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto instance = std::make_shared<KeySetManager>(*this);
	if (FAILED(instance->NativeConstruct(arg)))
	{
		return nullptr;
	}
	return instance;
}

auto KeySetManager::GetKeySet(int32_t index) -> std::shared_ptr<KeySetUi>
{
	return _key_sets[index];
}

auto KeySetManager::IsCollisionKeySet() const -> int32_t
{
	for (auto i = 0; i < 16; ++i)
	{
		if (_key_sets[i]->IsCollision())
		{
			return i;
		}
	}
	return -1;
}

auto KeySetManager::Create() -> std::shared_ptr<KeySetManager>
{
	auto instance = std::make_shared<KeySetManager>();
	if (FAILED(instance->NativeConstructPrototype()))
	{
		return nullptr;
	}
	return instance;
}

auto KeySetManager::AddComponents() -> HRESULT
{
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
	{
		return E_FAIL;
	}

	int keys[] = { DIK_Q, DIK_W, DIK_E,  DIK_R, DIK_A, DIK_S, DIK_D, DIK_F, DIK_1,  DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8 };
	auto info = GameLogicQueue::GetInstance()->GetCharacterInfo();
	for (int i = 0; i < 8; ++i)
	{
		auto pos = _float3(640.f + (i % 4 * 46.f) - 250, 340.f + (i >= 4 ? 46.f : 0.f) + 275, 0.f);
		_key_sets[i] = KeySetUi::Create(pos, i, keys[i]);
	}
	
	for (int i = 0; i < 8; ++i)
	{
		auto pos = _float3(1000.f + (i % 4 * 46.f) - 250, 340.f + (i >= 4 ? 46.f : 0.f) + 275, 0.f);
		_key_sets[i + 8] = KeySetUi::Create(pos, i + 8, keys[i + 8]);
	}

	auto baseInventory = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
	auto inventory = std::static_pointer_cast<Inventory>(baseInventory);
	if (inventory)
	{
		auto tabEqp = inventory->GetInventoryTab(Protocol::kInventoryEtc);
		for (auto key : info.keymap())
		{
			if (key.value() > 100)
			{
				auto baseSkillUi = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_SkillUi", 0);
				if (const auto skillUi = std::static_pointer_cast<SkillUi>(baseSkillUi))
				{
					_key_sets[key.index()]->SetSkill(skillUi->GetSkillSet(key.value()));
				}
			}
			else
			{
				auto item = tabEqp->FindItem(key.value());
				_key_sets[key.index()]->SetItem(item);
			}
		}
	}
	return S_OK;
}
