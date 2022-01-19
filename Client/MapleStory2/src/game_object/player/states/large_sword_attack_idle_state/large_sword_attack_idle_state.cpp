#include "c_pch.h"
#include "large_sword_attack_idle_state.h"

#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/jump_state/jump_state.h"
#include "src/game_object/player/states/large_sword_attack_run_state/large_sword_attack_run_state.h"
#include "src/game_object/player/states/large_sword_attack_state/large_sword_attack_state.h"
#include "src/game_object/player/states/move_state/move_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/transform/transform.h"

auto LargeSwordAttackIdleState::Enter() -> void
{
	_is_attack = false;
	_is_move = false;
	_player->ChangeAnimation(kAnimationType::kLargeSwordAttackIdle);
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

auto LargeSwordAttackIdleState::HandleInput() -> void
{
	if (!g_isWindowsActive)
	{
		return;
	}
	if (InputDevice::GetInstance().GetKeyPressing(DIK_UP))
	{
		_is_move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_DOWN))
	{
		_is_move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_LEFT))
	{
		_is_move = true;
	}
	else if (InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT))
	{
		_is_move = true;
	}
	if (InputDevice::GetInstance().GetKeyPressing(DIK_X))
	{
		_is_attack = true;
	}
	_is_skill_state = KeySetHandleInput();
	if (_is_skill_state.first != -1)
	{
		_is_skill_state.second->SetPushKey(_is_skill_state.first);
	}

	_is_jump = InputDevice::GetInstance().GetKeyPressing(DIK_C);
}

auto LargeSwordAttackIdleState::Tick(const double timeDelta) -> void
{
}

auto LargeSwordAttackIdleState::LateTick(const double timeDelta) -> void
{
	_player->PlayAnimation(timeDelta);
	auto check = GravityPlayer(timeDelta);
	if (check)
	{
		GravityPlayerSendMessage(kAnimationType::kIdle);
	}
	if (_is_skill_state.second != nullptr && _is_skill_state.first != -1)
	{
		_player->ChangeCharacterState(_is_skill_state.second);
		return;
	}
	if (_is_attack)
	{
		_player->ChangeCharacterState(LargeSwordAttackState::GetInstance());
		return;
	}
	if (_player->IsStartAttackTime())
	{
		_player->ChangeCharacterState(IdleState::GetInstance());
	}
	else
	{
		if (_is_move)
		{
			_player->ChangeCharacterState(LargeSwordAttackRunState::GetInstance());
		}
		if (_is_jump)
		{
			_player->ChangeCharacterState(JumpState::GetInstance());
		}
	}
}

auto LargeSwordAttackIdleState::Render() -> void
{
}
