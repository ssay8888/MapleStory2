#include "c_pch.h"
#include "idle_state.h"

#include "src/game_object/map/cube/map_object.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/jump_state/jump_state.h"
#include "src/game_object/player/states/move_state/move_state.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto IdleState::Enter() -> void
{
	// 상태가 바뀌었을때 최초 1회 호출됨.
	_is_move = false;
	_player->ChangeAnimation(Player::kAnimationType::kIdle);
}

auto IdleState::HandleInput() -> void
{
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
	_is_jump = InputDevice::GetInstance().GetKeyPressing(DIK_C);
}

auto IdleState::Tick(const double timeDelta) -> void
{
	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
}

auto IdleState::LateTick(const double timeDelta) -> void
{
	ReloadMapObject();
	GravityPlayer(timeDelta);
	_player->PlayAnimation(timeDelta);
	if (_is_move)
	{
		_player->ChangeCharacterState(MoveState::GetInstance());
	}
	if (_is_jump)
	{
		_player->ChangeCharacterState(JumpState::GetInstance());
	}
}

auto IdleState::Render() -> void
{
	for (const auto& object : _map_objects)
	{
		object->GetCollider()->RenderDebug();
	}
}
