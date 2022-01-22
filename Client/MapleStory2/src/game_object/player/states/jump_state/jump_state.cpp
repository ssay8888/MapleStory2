#include "c_pch.h"
#include "jump_state.h"

#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/ladder_state/ladder_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/timers/timer_manager.h"

auto JumpState::Enter() -> void
{
	auto& timerManager = TimerManager::GetInstance();
	timerManager.AddTimers(L"JumpTimerUp");
	timerManager.ComputeTimeDelta(L"JumpTimerUp");
	timerManager.ResetTime(TEXT("JumpTimerUp"));
	_player->ChangeAnimation(_player->IsStartAttackTime() ? kAnimationType::kJumpUp : kAnimationType::kLargeSwordJumpUp);
	_is_jump_up = true;
	_is_jump_down = false;
	_is_ladder = false;
}

auto JumpState::HandleInput() -> void
{
	_is_move = false;
	_is_up_key = false;
	_is_ladder = false;
	if (!g_isWindowsActive)
	{
		return;
	}
	if (!_is_jump_up && !_is_jump_down)
	{
		_is_jump_up = InputDevice::GetInstance().GetKeyPressing(DIK_C);
	}
	const auto upKey = InputDevice::GetInstance().GetKeyPressing(DIK_UP);
	const auto downKey = InputDevice::GetInstance().GetKeyPressing(DIK_DOWN);
	const auto leftKey = InputDevice::GetInstance().GetKeyPressing(DIK_LEFT);
	const auto rightKey = InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT);
	if (upKey && leftKey)
	{
		_player->SetRadian(D3DXToRadian(135));
		_is_move = true;
		_is_up_key = true;
	}
	else if (upKey && rightKey)
	{
		_player->SetRadian(D3DXToRadian(225));
		_is_move = true;
		_is_up_key = true;
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
			_is_up_key = true;
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
}

auto JumpState::Tick(const double timeDelta) -> void
{
	const auto transform = _player->GetTransform();
	if (_is_jump_up)
	{
		auto& timerManager = TimerManager::GetInstance();
		if (timerManager.IsTimeCheck(L"JumpTimerUp", _jump_up_time))
		{
			_is_jump_up = false;
			_is_jump_down = true;
			_player->ChangeAnimation(_player->IsStartAttackTime() ? kAnimationType::kJumpDown : kAnimationType::kLargeSwordJumpDown);
			timerManager.ResetTime(TEXT("JumpTimerUp"));
		}
		else
		{

			auto playerPos = transform->GetState(Transform::kState::kStatePosition);

			playerPos.y += static_cast<float>(_jump_power * timeDelta);

			transform->SetState(Transform::kState::kStatePosition, playerPos);

		}
	}
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
			if (!_is_jump_up)
			{
				if (_is_up_key && LadderBlockCheck())
				{
					_is_ladder = true;
					transform->BackStraight(-timeDelta);
				}
				else
				{
					transform->BackStraight(-timeDelta);
				}
			}
			else
			{
				transform->BackStraight(-timeDelta);
			}
		}
	}



	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
}

auto JumpState::LateTick(const double timeDelta) -> void
{

	if (_is_jump_up)
	{
		_player->PlayAnimation(0.008580);
	}
	else
	{
		_player->PlayAnimation(0.004290);
	}
	bool gravity = true;
	if (_is_jump_down)
	{
		gravity = GravityPlayer(timeDelta);
	}
	ReloadMapObject();

	if (gravity  || _is_jump_up)
	{
		const auto transform = _player->GetTransform();
		Protocol::GameClientMovePlayer sendPkt;
		if (_is_jump_up)
		{
			sendPkt.set_state(static_cast<int32_t>(_player->IsStartAttackTime() ? kAnimationType::kJumpUp : kAnimationType::kLargeSwordJumpUp));
		}
		else
		{
			sendPkt.set_state(static_cast<int32_t>(_player->IsStartAttackTime() ? kAnimationType::kJumpDown : kAnimationType::kLargeSwordJumpDown));
		}
		sendPkt.set_radian(_player->GetRadian());
		const auto playerPos = transform->GetState(Transform::kState::kStatePosition);
		const auto position = sendPkt.mutable_position();
		position->set_x(playerPos.x);
		position->set_y(playerPos.y);
		position->set_z(playerPos.z);
		SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
	}

	if (!_is_jump_up && !gravity)
	{
		_player->ChangeCharacterState(IdleState::GetInstance());
	}

	if (_is_ladder)
	{
		_player->ChangeCharacterState(LadderState::GetInstance());
	}
}

auto JumpState::Render() -> void
{
	for (const auto& object : _map_objects)
	{
		object->GetCollider()->RenderDebug();
	}
}
