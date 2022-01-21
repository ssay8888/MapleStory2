#include "c_pch.h"
#include "game_logic_queue.h"

#include "src/game_object/item/Item.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/monster/monster.h"
#include "src/game_object/monster/states/attack_a_state/monster_attack_a_state.h"
#include "src/game_object/monster/states/run_state/monster_run_state.h"
#include "src/game_object/monster/states/walk_state/monster_walk_state.h"
#include "src/game_object/monster/stats/monster_stats.h"
#include "src/game_object/numbers/numbers.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/down_idle_state/down_idle_state.h"
#include "src/game_object/player/states/down_state/down_state.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/ui/chat_ui/chat_ui.h"
#include "src/game_object/ui/inventory/inventory_ui.h"
#include "src/game_object/ui/inventory/inventory_tab_btn/inventory_tab_btn.h"
#include "src/game_object/user/user.h"
#include "src/managers/character_stat/character_stat.h"
#include "src/managers/weapon_manager/weapon_manager.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"
#include "string_utils/string_utils.h"

auto GameLogicQueue::GetCharacterInfo() const -> Protocol::GameServerLoadCharacter
{
	return _character_info;
}

auto GameLogicQueue::CharacterLoad(PacketSessionRef session, Protocol::GameServerLoadCharacter pkt) -> void
{
	const auto scene = SceneLoading::Create(GraphicDevice::GetInstance().GetDevice(), kSceneGamePlay0);
	if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
	{
		_character_info = pkt;
	}
}

auto GameLogicQueue::AddUser(PacketSessionRef session, Protocol::GameServerRespawnPlayer pkt)->void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer = fmt::format(L"Layer_User_{}", pkt.character_id());
	objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_User"), layer, &pkt);
}

auto GameLogicQueue::MovePlayer(PacketSessionRef session, Protocol::GameServerMovePlayer pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer = fmt::format(L"Layer_User_{}", pkt.character_id());
	const auto userObject = std::static_pointer_cast<User>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
	if (userObject)
	{
		auto transform = std::static_pointer_cast<Transform>(userObject->GetComponentPtr(L"Com_Transform"));
		transform->SetState(Transform::kState::kStatePosition, _float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), pkt.radian());
		userObject->ChangeAnimation(static_cast<kAnimationType>(pkt.state()));
	}
}

auto GameLogicQueue::RespawnMonster(PacketSessionRef session, Protocol::GameServerRespawnMonster pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();

	wchar_t LayerTag[MAX_PATH];
	swprintf_s(LayerTag, L"Layer_Monster_%lld", pkt.object_id());
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Mesh_Monster"), LayerTag, &pkt)))
	{
		GetInstance()->DoAsync(&GameLogicQueue::RespawnMonster, session, pkt);
	}
	else
	{
		if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad"))
		{
			if (const auto monster = std::static_pointer_cast<Monster>(objectManager.GetGameObjectPtr(kSceneGamePlay0, LayerTag, 0)))
			{
				mapInstance->AddMonster(pkt.object_id(), monster);
			}
		}
	}
}

auto GameLogicQueue::MoveMonster(PacketSessionRef session, Protocol::GameServerMoveMonster pkt)->void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer = fmt::format(L"Layer_Monster_{}", pkt.object_id());
	const auto monsterObject = std::static_pointer_cast<Monster>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
	if (monsterObject)
	{
		switch (pkt.state())
		{
		case Protocol::kIdleA:
		case Protocol::kBoreA:
		{
			auto transform = std::static_pointer_cast<Transform>(monsterObject->GetComponentPtr(L"Com_Transform"));
			if (monsterObject->GetCurrentState() == monsterObject->GetMonsterState(Protocol::kWalkA))
			{

				if (const auto monsterWalkState = std::static_pointer_cast<MonsterWalkState>(monsterObject->GetMonsterState(Protocol::kWalkA)))
				{
					monsterWalkState->SetNextState(pkt);
				}

			}
			else if (monsterObject->GetCurrentState() == monsterObject->GetMonsterState(Protocol::kRunA))
			{
				if (const auto monsterRunState = std::static_pointer_cast<MonsterRunState>(monsterObject->GetMonsterState(Protocol::kRunA)))
				{
					monsterRunState->SetNextState(pkt);
				}
			}
			else
			{
				transform->SetState(Transform::kState::kStateRight, _float3(pkt.right().x(), pkt.right().y(), pkt.right().z()));
				transform->SetState(Transform::kState::kStateUp, _float3(pkt.up().x(), pkt.up().y(), pkt.up().z()));
				transform->SetState(Transform::kState::kStateLook, _float3(pkt.look().x(), pkt.look().y(), pkt.look().z()));
				transform->SetState(Transform::kState::kStatePosition, _float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
				monsterObject->ChangeAnimation(pkt.state());
			}
		}
		break;
		case Protocol::kWalkA:
		{

			const auto monsterState = std::static_pointer_cast<MonsterWalkState>(monsterObject->GetMonsterState(Protocol::kWalkA));
			if (monsterState)
			{
				auto transform = std::static_pointer_cast<Transform>(monsterObject->GetComponentPtr(L"Com_Transform"));
				transform->SetState(Transform::kState::kStateRight, _float3(pkt.right().x(), pkt.right().y(), pkt.right().z()));
				transform->SetState(Transform::kState::kStateUp, _float3(pkt.up().x(), pkt.up().y(), pkt.up().z()));
				transform->SetState(Transform::kState::kStateLook, _float3(pkt.look().x(), pkt.look().y(), pkt.look().z()));
				monsterState->SetTargetPos(_float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
				monsterObject->ChangeAnimation(pkt.state());
			}
		}
		break;
		case Protocol::kRegenA: break;
		case Protocol::kRunA:
		{
			const auto monsterState = std::static_pointer_cast<MonsterRunState>(monsterObject->GetMonsterState(Protocol::kRunA));
			auto transform = std::static_pointer_cast<Transform>(monsterObject->GetComponentPtr(L"Com_Transform"));
			transform->SetState(Transform::kState::kStateRight, _float3(pkt.right().x(), pkt.right().y(), pkt.right().z()));
			transform->SetState(Transform::kState::kStateUp, _float3(pkt.up().x(), pkt.up().y(), pkt.up().z()));
			transform->SetState(Transform::kState::kStateLook, _float3(pkt.look().x(), pkt.look().y(), pkt.look().z()));
			monsterState->SetTargetPos(_float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
			if (monsterObject->GetCurrentState() != monsterState)
			{
				monsterObject->ChangeAnimation(pkt.state());
			}
		}
		break;
		case Protocol::kDeadA: break;
		case Protocol::kDeadB: break;
		case Protocol::kAttack1A:
		case Protocol::kAttack2A:
		case Protocol::kAttack3A:
		case Protocol::kAttack1B:
		case Protocol::kAttack2B:
		case Protocol::kAttack3B:
		case Protocol::kAttack1C:
		case Protocol::kAttack2C:
		case Protocol::kAttack3C:
		{
			const auto transform = std::static_pointer_cast<Transform>(monsterObject->GetComponentPtr(L"Com_Transform"));
			transform->SetState(Transform::kState::kStateRight, _float3(pkt.right().x(), pkt.right().y(), pkt.right().z()));
			transform->SetState(Transform::kState::kStateUp, _float3(pkt.up().x(), pkt.up().y(), pkt.up().z()));
			transform->SetState(Transform::kState::kStateLook, _float3(pkt.look().x(), pkt.look().y(), pkt.look().z()));
			monsterObject->ChangeSkillId(pkt.skillid());
			monsterObject->ChangeAnimation(pkt.state());
			for (auto state = static_cast<int32_t>(Protocol::kAttack1A); state <= static_cast<int32_t>(Protocol::kAttack3C); ++state)
			{
				const auto monsterState = std::static_pointer_cast<MonsterAttackAState>(monsterObject->GetMonsterState(static_cast<Protocol::kMonsterState>(state)));
				if (monsterState)
				{
					monsterState->SetTargetPos(_float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
				}
			}
		}
		break;
		case Protocol::kDamgA: break;
		case Protocol::kDamgB: break;
		case Protocol::kDamgC: break;
		case Protocol::kMonsterState_INT_MIN_SENTINEL_DO_NOT_USE_: break;
		case Protocol::kMonsterState_INT_MAX_SENTINEL_DO_NOT_USE_: break;
		default:;
		}
	}
}

auto GameLogicQueue::UpdateStat(PacketSessionRef session, Protocol::GameServerUpdateStat pkt) -> void
{
	switch (pkt.type())
	{
	case Protocol::kHp:
	{
		CharacterStat::GetInstance().SetHp(pkt.value());

		auto& objectManager = ObjectManager::GetInstance();
		const auto playerObject = std::static_pointer_cast<Player>(objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0));
		if (playerObject)
		{
			if (playerObject->GetCharacterState() != DownState::GetInstance() &&
				playerObject->GetCharacterState() != DownIdleState::GetInstance())
			{
				if (CharacterStat::GetInstance().IsDead())
				{
					playerObject->ChangeCharacterState(DownState::GetInstance());
				}
			}
		}
		break;
	}
	case Protocol::kMp:
		CharacterStat::GetInstance().SetMp(pkt.value());
		break;
	case Protocol::kExp:
		CharacterStat::GetInstance().SetExp(pkt.value());
		break;
	default:;
	}
}

auto GameLogicQueue::AttackMonster(PacketSessionRef session, Protocol::GameServerAttackMonster pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();
	const auto playerObject = std::static_pointer_cast<Player>(objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0));
	if (playerObject)
	{

		if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad"))
		{
			for (auto& damageItem : pkt.damages())
			{
				const auto monster = mapInstance->FindMonster(damageItem.monster_obj_id());
				if (monster)
				{
					Numbers::NumberInfo info;
					info.is_player_attack = true;
					info.numbers = damageItem.damage();
					info.parent_transform = monster->GetTransform();
					if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Numbers"), L"Layer_Numbers", &info)))
					{
						return;
					}
				}
			}

		}
	}
}

auto GameLogicQueue::UpdateMonsterStat(PacketSessionRef session, Protocol::GameServerMonsterStatUpdate pkt) -> void
{
	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad"))
	{
		const auto monster = mapInstance->FindMonster(pkt.monster_obj_id());

		switch (pkt.type())
		{
		case Protocol::kMonsterHp:
			monster->GetStat()->SetHp(pkt.value());
			break;
		case Protocol::kMonsterMp:
			break;
		default:;
		}
	}
}

auto GameLogicQueue::KillMonster(PacketSessionRef session, Protocol::GameServerKillMonster pkt) -> void
{
	if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad"))
	{
		auto monster = mapInstance->FindMonster(pkt.monster_obj_id());
		monster->GetStat()->SetHp(0);
		monster->ChangeAnimation(Protocol::kDeadA);
	}
}

auto GameLogicQueue::TakeDamage(PacketSessionRef session, Protocol::GameServerTakeDamage pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();
	if (pkt.character_id() == _character_info.character_id())
	{
		const auto playerObject = std::static_pointer_cast<Player>(objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0));
		if (playerObject)
		{
			Numbers::NumberInfo info;
			info.is_player_attack = false;
			info.numbers = pkt.damage();
			info.parent_transform = playerObject->GetTransform();
			if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Numbers"), L"Layer_Numbers", &info)))
			{
				return;
			}
		}
	}
	else
	{
		std::wstring layer = fmt::format(L"Layer_User_{}", pkt.character_id());
		const auto userObject = std::static_pointer_cast<Monster>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
		if (userObject)
		{
			Numbers::NumberInfo info;
			info.is_player_attack = false;
			info.numbers = pkt.damage();
			info.parent_transform = userObject->GetTransform();
			if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Numbers"), L"Layer_Numbers", &info)))
			{
				return;
			}
		}
	}

}

auto GameLogicQueue::DressChange(PacketSessionRef session, Protocol::GameServerDressChange pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer = fmt::format(L"Layer_User_{}", pkt.character_id());
	const auto userObject = std::static_pointer_cast<User>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
	if (userObject)
	{
		auto type = static_cast<GameContents::kEquipeType>(pkt.item_type());
		if (type == GameContents::kEquipeType::kWeapon)
		{
			if (pkt.item_id() == 0)
			{
				WeaponManager::GetInstance().RemoveWeapon(pkt.character_id());
			}
			else
			{
				auto tarnsform = std::static_pointer_cast<Transform>(userObject->GetComponentPtr(L"Com_Transform"));
				WeaponManager::GetInstance().AddWeapon(pkt.character_id(), pkt.item_id(), tarnsform);
			}
		}
		else
		{
			userObject->ChangeEqp(static_cast<GameContents::kEquipeType>(pkt.item_type()), pkt.item_id());
		}
	}
}

auto GameLogicQueue::StatupChange(PacketSessionRef session, Protocol::GameServerStatUp pkt) -> void
{
	switch (pkt.type())
	{
	case Protocol::kStr:
		CharacterStat::GetInstance().SetStr(pkt.value());
		break;
	case Protocol::kDex:
		CharacterStat::GetInstance().SetDex(pkt.value());
		break;
	case Protocol::kInt:
		CharacterStat::GetInstance().SetInt(pkt.value());
		break;
	case Protocol::kLuk:
		CharacterStat::GetInstance().SetLuk(pkt.value());
		break;
	default:;
	}
	CharacterStat::GetInstance().SetAp(pkt.ap_value());
}

auto GameLogicQueue::ItemQuantityUpdate(PacketSessionRef session, Protocol::GameServerItemQuantityUpdate pkt) -> void
{
	auto baseInventory = ObjectManager::GetInstance().GetGameObjectPtr(kSceneGamePlay0, L"Layer_Inventory", 0);
	auto inventory = std::static_pointer_cast<Inventory>(baseInventory);
	if (inventory)
	{
		auto tabEqp = inventory->GetInventoryTab(pkt.type());
		if (pkt.quantity() <= 0)
		{
			tabEqp->RemoveItem(pkt.position());
			return;
		}
		auto item = tabEqp->FindItem(pkt.position());
		if (item)
		{
			item->SetItemQuantity(pkt.quantity());
		}
	}
}

auto GameLogicQueue::ResurrectionPlayer(PacketSessionRef session, Protocol::GameServerResurrection pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();

	if (_character_info.character_id() == pkt.character_id())
	{
		const auto playerObject = std::static_pointer_cast<Player>(objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_Character", 0));
		if (playerObject)
		{
			playerObject->GetTransform()->SetState(Transform::kState::kStatePosition, _float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
			playerObject->ChangeCharacterState(IdleState::GetInstance());
		}
	}
	else
	{
		std::wstring layer = fmt::format(L"Layer_User_{}", pkt.character_id());
		const auto userObject = std::static_pointer_cast<User>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
		if (userObject)
		{
			auto transform = std::static_pointer_cast<Transform>(userObject->GetComponentPtr(L"Com_Transform"));
			transform->SetState(Transform::kState::kStatePosition, _float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
			userObject->ChangeAnimation(kAnimationType::kIdle);
		}
	}
}

auto GameLogicQueue::GameChat(PacketSessionRef session, Protocol::GameServerChat pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();

	const auto chatUi = std::static_pointer_cast<ChatUi>(objectManager.GetGameObjectPtr(kSceneGamePlay0, L"Layer_ChatUi", 0));
	if (chatUi)
	{
		chatUi->PushHistory(StringUtils::ConvertCtoW(pkt.contents().c_str()));
	}
}

