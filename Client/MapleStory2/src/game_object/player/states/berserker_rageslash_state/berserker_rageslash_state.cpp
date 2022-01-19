#include "c_pch.h"
#include "berserker_rageslash_state.h"

#include "protocol/game_protocol.pb.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/monster/monster.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/large_sword_attack_idle_state/large_sword_attack_idle_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/transform/transform.h"

auto BerserkerRageslashState::Enter() -> void
{
	_is_move = false;
	_input_combo = false;
	_moves.clear();
	_player->SettingStartAttackTime();
	const auto gender = _player->GetInfo().sex;
	const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);
	switch (_player->GetCurrentAnimationType())
	{
	case kAnimationType::kBerserkerRageslash1A:
		_seq = kfm->seqs[gender == false ? 434 : 477];
		_player->ChangeAnimation(kAnimationType::kBerserkerRageslash1A);
		break;
	case kAnimationType::kBerserkerRageslash2A:
		_seq = kfm->seqs[gender == false ? 456 : 476];
		_player->ChangeAnimation(kAnimationType::kBerserkerRageslash2A);
		break;
	case kAnimationType::kBerserkerRageslash3A:
		_seq = kfm->seqs[gender == false ? 435 : 465];
		_player->ChangeAnimation(kAnimationType::kBerserkerRageslash3A);
		break;
	default:
		_seq = kfm->seqs[gender == false ? 434 : 477];
		_player->ChangeAnimation(kAnimationType::kBerserkerRageslash1A);
		break;;
	}

	if (_aabb_com == nullptr)
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

	if (_origin_moves.empty())
	{
		for (int i = 0; i < 4; i++)
		{
			auto iterator = _seq->key.find(fmt::format(L"move{}", i));
			if (iterator != _seq->key.end())
			{
				_origin_moves.emplace(iterator->first, iterator->second);
			}
			else
			{
				break;
			}
		}
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

auto BerserkerRageslashState::HandleInput() -> void
{
	if (_input_combo)
	{
		return;
	}

	const auto upKey = InputDevice::GetInstance().GetKeyPressing(DIK_UP);
	const auto downKey = InputDevice::GetInstance().GetKeyPressing(DIK_DOWN);
	const auto leftKey = InputDevice::GetInstance().GetKeyPressing(DIK_LEFT);
	const auto rightKey = InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT);
	if (upKey && leftKey)
	{
		_player->SetRadian(D3DXToRadian(135));
		_is_move = true;
	}
	else if (upKey && rightKey)
	{
		_player->SetRadian(D3DXToRadian(225));
		_is_move = true;
	}
	else if (downKey && leftKey)
	{
		_player->SetRadian(D3DXToRadian(45));
		_is_move = true;
	}
	else if (downKey && rightKey)
	{
		_player->SetRadian(D3DXToRadian(315));
		_is_move = true;
	}
	else
	{
		if (upKey)
		{
			_player->SetRadian(D3DXToRadian(180));
			_is_move = true;
		}
		if (leftKey)
		{
			_player->SetRadian(D3DXToRadian(90));
			_is_move = true;
		}
		if (downKey)
		{
			_player->SetRadian(D3DXToRadian(0));
			_is_move = true;
		}
		if (rightKey)
		{
			_player->SetRadian(D3DXToRadian(270));
			_is_move = true;
		}
	}
	
	auto inputComboTime = _seq->key[L"inputcombo"];
	if (_player->GetAnimationTimeAcc() >= inputComboTime)
	{
		if (InputDevice::GetInstance().GetKeyPressing(_key))
		{
			_input_combo = true;
		}
	}
}

auto BerserkerRageslashState::Tick(const double timeDelta) -> void
{
	_aabb_com->UpdateCollider();


	if (_is_move)
	{
		for (auto [key, time] : _origin_moves)
		{
			auto iter = _moves.find(key);
			if (_player->GetAnimationTimeAcc() >= time && iter == _moves.end())
			{
				_moves.emplace(key, time);
				const auto transform = _player->GetTransform();
				transform->SetUpRotation(_float3(0.f, 1.f, 0.f), _player->GetRadian());
				transform->BackStraight(timeDelta * 6);

				_player->GetCharacterColliderAabb()->UpdateCollider();
				auto range = _player->GetReloadRangeAabb();
				for (const auto& reload : range)
				{
					reload->UpdateCollider();
				}

				if (StraightCheck())
				{
					transform->BackStraight(-timeDelta * 6);
				}
				else
				{
					const auto transform = _player->GetTransform();
					Protocol::GameClientMovePlayer sendPkt;
					sendPkt.set_state(static_cast<google::protobuf::int32>(_player->GetCurrentAnimationType()));
					sendPkt.set_radian(_player->GetRadian());
					const auto playerPos = transform->GetState(Transform::kState::kStatePosition);
					const auto position = sendPkt.mutable_position();
					position->set_x(playerPos.x);
					position->set_y(playerPos.y);
					position->set_z(playerPos.z);
					SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
				}

				_player->GetCharacterColliderAabb()->UpdateCollider();
				for (const auto& reload : range)
				{
					reload->UpdateCollider();
				}
			}
		}
	}

	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
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
				}
			}
			SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
		}
	}
}

auto BerserkerRageslashState::LateTick(const double timeDelta) -> void
{
	switch (_player->GetCurrentAnimationType())
	{

	case kAnimationType::kBerserkerRageslash1A:
		_player->PlayAnimation(timeDelta * 1.3);
		break;
	case kAnimationType::kBerserkerRageslash2A:
		_player->PlayAnimation(timeDelta * 1.55);
		break;
	case kAnimationType::kBerserkerRageslash3A:
		_player->PlayAnimation(timeDelta * 2.05);
		break;
	default:
		break;
	}

	//const auto gender = _player->GetInfo().sex;
	//const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);

	auto check = GravityPlayer(timeDelta);
	if (check)
	{
		GravityPlayerSendMessage(_player->GetCurrentAnimationType());
	}
	ReloadMapObject();
	auto endTime = 0.f;
	switch (_player->GetCurrentAnimationType())
	{
	case kAnimationType::kBerserkerRageslash1A:
	case kAnimationType::kBerserkerRageslash2A:
		endTime = _seq->key[L"end"] - (_input_combo ? _seq->key[L"pauseEvent"] : 0);
		break;
	case kAnimationType::kBerserkerRageslash3A:
		endTime = _seq->key[L"end"];
		break;
	default:
		return;
	}
	if (_player->GetAnimationTimeAcc() >= endTime)
	{
		_monsters.clear();
		if (_input_combo)
		{
			switch (_player->GetCurrentAnimationType())
			{
			case kAnimationType::kBerserkerRageslash1A:
				_player->ChangeAnimation(kAnimationType::kBerserkerRageslash2A);
				_player->ChangeCharacterState(GetInstance());
				break;
			case kAnimationType::kBerserkerRageslash2A:
				_player->ChangeAnimation(kAnimationType::kBerserkerRageslash3A);
				_player->ChangeCharacterState(GetInstance());
				break;
			case kAnimationType::kBerserkerRageslash3A:
				_player->ChangeAnimation(kAnimationType::kBerserkerRageslash3A);
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

auto BerserkerRageslashState::Render() -> void
{
	_aabb_com->RenderDebug();
}
