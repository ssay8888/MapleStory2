#include "c_pch.h"
#include "move_state.h"

#include "protocol/game_protocol.pb.h"
#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/jump_state/jump_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto MoveState::Enter() -> void
{
	_player->ChangeAnimation(kAnimationType::kRun);
}

auto MoveState::HandleInput() -> void
{
	_is_move = false;
	if (!g_isWindowsActive)
	{
		return;
	}
	const auto upKey = InputDevice::GetInstance().GetKeyPressing(DIK_UP);
	const auto downKey = InputDevice::GetInstance().GetKeyPressing(DIK_DOWN);
	const auto leftKey = InputDevice::GetInstance().GetKeyPressing(DIK_LEFT);
	const auto rightKey = InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT);
	_is_jump = InputDevice::GetInstance().GetKeyPressing(DIK_C);
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
	_is_skill_state = KeySetHandleInput();
	if (_is_skill_state.first != -1)
	{
		_is_skill_state.second->SetPushKey(_is_skill_state.first);
	}
}

auto MoveState::Tick(const double timeDelta) -> void
{
	auto transform = _player->GetTransform();
	if (_is_move)
	{
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), _player->GetRadian());
		transform->BackStraight(timeDelta);

		_player->GetCharacterColliderAabb()->UpdateCollider();
		auto range = _player->GetReloadRangeAabb();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}

		if (StraightCheck())
		{
			transform->BackStraight(-timeDelta);
		}
		else
		{

			const auto transform = _player->GetTransform();
			Protocol::GameClientMovePlayer sendPkt;
			sendPkt.set_state(static_cast<int32_t>(kAnimationType::kRun));
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

auto MoveState::LateTick(const double timeDelta) -> void
{
	_player->PlayAnimation(timeDelta);
	if (_is_skill_state.second != nullptr && _is_skill_state.first != -1)
	{
		_player->ChangeCharacterState(_is_skill_state.second);
		return;
	}

	auto check = GravityPlayer(timeDelta);
	if (check)
	{
		GravityPlayerSendMessage(kAnimationType::kRun);
	}
	ReloadMapObject();
	if (!_is_move)
	{
		_player->ChangeCharacterState(IdleState::GetInstance());
	}
	if (_is_jump)
	{
		_player->ChangeCharacterState(JumpState::GetInstance());
	}
}

auto MoveState::Render() -> void
{
	for (const auto& object : _map_objects)
	{
		object->GetCollider()->RenderDebug();
	}
}