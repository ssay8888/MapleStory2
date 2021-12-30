#include "c_pch.h"
#include "move_state.h"

#include "src/game_object/map/map_instance.h"
#include "src/game_object/map/map_manager.h"
#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/jump_state/jump_state.h"
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
	const auto upKey = InputDevice::GetInstance().GetKeyPressing(DIK_UP);
	const auto downKey = InputDevice::GetInstance().GetKeyPressing(DIK_DOWN);
	const auto leftKey = InputDevice::GetInstance().GetKeyPressing(DIK_LEFT);
	const auto rightKey = InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT);
	_is_jump = InputDevice::GetInstance().GetKeyPressing(DIK_C);
	if (upKey && leftKey)
	{
		_radian = D3DXToRadian(135);
		_is_move = true;
	}
	else if (upKey && rightKey)
	{
		_radian = D3DXToRadian(225);
		_is_move = true;
	}
	else if (downKey && leftKey)
	{
		_radian = D3DXToRadian(45);
		_is_move = true;
	}
	else if (downKey && rightKey)
	{
		_radian = D3DXToRadian(315);
		_is_move = true;
	}
	else
	{
		if (upKey)
		{
			_radian = D3DXToRadian(180);
			_is_move = true;
		}
		if (leftKey)
		{
			_radian = D3DXToRadian(90);
			_is_move = true;
		}
		if (downKey)
		{
			_radian = D3DXToRadian(0);
			_is_move = true;
		}
		if (rightKey)
		{
			_radian = D3DXToRadian(270);
			_is_move = true;
		}
	}
}

auto MoveState::Tick(const double timeDelta) -> void
{
	auto transform = _player->GetTransform();
	if (_is_move)
	{
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), _radian);
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

	GravityPlayer(timeDelta);
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