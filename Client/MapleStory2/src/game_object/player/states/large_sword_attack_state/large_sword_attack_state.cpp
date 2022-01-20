#include "c_pch.h"
#include "large_sword_attack_state.h"

#include "data_reader/data_reader_manager.h"
#include "protocol/game_enum.pb.h"
#include "randomizer/randomizer.h"
#include "src/game_job_queue/game_logic_queue.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/monster/monster.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/large_sword_attack_idle_state/large_sword_attack_idle_state.h"
#include "src/game_object/ui/monster_hp_ui/monster_hp_ui.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"

auto LargeSwordAttackState::Enter() -> void
{
	_input_combo = false;
	_player->SettingStartAttackTime();
	const auto gender = _player->GetInfo().sex;
	const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);
	switch (_player->GetCurrentAnimationType())
	{
	case kAnimationType::kLargeSwordAttack1A:
		_seq = kfm->seqs[gender == false ? 131 : 83];
		_player->ChangeAnimation(kAnimationType::kLargeSwordAttack1A);
		break;
	case kAnimationType::kLargeSwordAttack2A:
		_seq = kfm->seqs[gender == false ? 132 : 84];
		_player->ChangeAnimation(kAnimationType::kLargeSwordAttack2A);
		break;
	case kAnimationType::kLargeSwordAttack3A:
		_seq = kfm->seqs[gender == false ? 82 : 224];
		_player->ChangeAnimation(kAnimationType::kLargeSwordAttack3A);
		break;
	default:
		_seq = kfm->seqs[gender == false ? 131 : 83];
		_player->ChangeAnimation(kAnimationType::kLargeSwordAttack1A);
		break;;
	}

	if (_aabb_com == nullptr )
	{
		const auto transform = _player->GetTransform();
		Collider::TagColliderDesc		ColliderDesc;
		ColliderDesc.parent_matrix = &transform->GetWorldMatrix();
		ColliderDesc.scale = _float3(0.4f, 0.4f, 0.4f);
		ColliderDesc.init_pos = _float3(0.f, 0.2f, -0.2f);

		const auto& componentManager = ComponentManager::GetInstance();
		auto component = componentManager.CloneComponent(0, TEXT("Prototype_Collider_AABB"), &ColliderDesc);
		_aabb_com = std::static_pointer_cast<Collider>(component);
		_aabb_com->NativeConstruct(&ColliderDesc);
		_aabb_com->UpdateCollider();
	}
	Protocol::GameClientMovePlayer sendPkt;
	sendPkt.set_state(static_cast<int32_t>(_player->GetCurrentAnimationType()));
	sendPkt.set_radian(_player->GetRadian());
	const auto playerPos = _player->GetTransform()->GetState(Transform::kState::kStatePosition);
	const auto position = sendPkt.mutable_position();
	position->set_x(playerPos.x);
	position->set_y(playerPos.y);
	position->set_z(playerPos.z);
	SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
}

auto LargeSwordAttackState::HandleInput() -> void
{
	if (_input_combo)
	{
		return;
	}

	auto inputComboTime = _seq->key[L"inputcombo"];
	if (_player->GetAnimationTimeAcc() >= inputComboTime)
	{
		if (InputDevice::GetInstance().GetKeyPressing(DIK_X))
		{
			_input_combo = true;
		}
	}
}

auto LargeSwordAttackState::Tick(const double timeDelta) -> void
{
	_aabb_com->UpdateCollider();


	auto p0 = _seq->key[L"p0"];
	if (_player->GetAnimationTimeAcc() >= p0 && _monsters.empty())
	{
		if (const auto mapInstance = MapManager::GetInstance().FindMapInstance(L"02000003_ad"))
		{
			const auto monsters = mapInstance->CollisionMonsters(_aabb_com);
			Protocol::GameClientAttackMonster sendPkt;
			const auto objectIds = sendPkt.mutable_monster_obj_id();
			for (const auto& monster : monsters)
			{
				auto iterator = _monsters.find(monster->GetMonsterInfo().object_id());
				if (iterator == _monsters.end())
				{
					objectIds->Add(monster->GetMonsterInfo().object_id());
					_monsters.emplace(monster->GetMonsterInfo().object_id(), monster);
					MonsterHpUi::GetInstance()->SetMonster(monster);
				}
			}
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
		}
	}
}

auto LargeSwordAttackState::LateTick(const double timeDelta) -> void
{
	_player->PlayAnimation(timeDelta);

	const auto gender = _player->GetInfo().sex;
	const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);

	const auto endTime = _seq->key[L"end"] - (_input_combo ? _seq->key[L"pauseEvent"] : 0);
	if (_player->GetAnimationTimeAcc() >= endTime)
	{
		_monsters.clear();
		if (_input_combo)
		{
			switch (_player->GetCurrentAnimationType())
			{
			case kAnimationType::kLargeSwordAttack1A:
				_player->ChangeAnimation(kAnimationType::kLargeSwordAttack2A);
				_player->ChangeCharacterState(GetInstance());
				break;
			case kAnimationType::kLargeSwordAttack2A:
				_player->ChangeAnimation(kAnimationType::kLargeSwordAttack3A);
				_player->ChangeCharacterState(GetInstance());
				break;
			case kAnimationType::kLargeSwordAttack3A:
				_player->ChangeAnimation(kAnimationType::kLargeSwordAttack1A);
				_player->ChangeCharacterState(GetInstance());
				break;
			default:
				return;
			}

		}
		else
		{
			Protocol::GameClientMovePlayer sendPkt;
			sendPkt.set_state(static_cast<int32_t>(kAnimationType::kLargeSwordAttackIdle));
			sendPkt.set_radian(_player->GetRadian());
			const auto playerPos = _player->GetTransform()->GetState(Transform::kState::kStatePosition);
			const auto position = sendPkt.mutable_position();
			position->set_x(playerPos.x);
			position->set_y(playerPos.y);
			position->set_z(playerPos.z);
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
			_player->ChangeAnimation(kAnimationType::kLargeSwordAttackIdle);
			_player->ChangeCharacterState(LargeSwordAttackIdleState::GetInstance());
		}
	}
}

auto LargeSwordAttackState::Render() -> void
{
	_aabb_com->RenderDebug();
}
