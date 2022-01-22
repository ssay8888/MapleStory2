#include "c_pch.h"
#include "ladder_state.h"

#include "data_reader/data_reader_manager.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/game_object/player/states/jump_state/jump_state.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto LadderState::Enter() -> void
{
	_player->ChangeAnimation(kAnimationType::kLadderIdle);
	_is_jump = false;
}

auto LadderState::HandleInput() -> void
{
	_is_left = false;
	_is_right = false;
	_is_up = false;
	_is_down = false;
	_is_upper_left = false;
	_is_upper_right = false;
	_is_bottom_left = false;
	_is_bottom_right = false;

	if (InputDevice::GetInstance().GetKeyDown(DIK_C))
	{
		_is_jump = true;
	}

	const auto upKey = InputDevice::GetInstance().GetKeyPressing(DIK_UP);
	const auto downKey = InputDevice::GetInstance().GetKeyPressing(DIK_DOWN);
	const auto leftKey = InputDevice::GetInstance().GetKeyPressing(DIK_LEFT);
	const auto rightKey = InputDevice::GetInstance().GetKeyPressing(DIK_RIGHT);
	if (upKey && leftKey)
	{
		_is_upper_left = true;
	}
	else if (upKey && rightKey)
	{
		_is_upper_right = true;
	}
	else if (downKey && leftKey)
	{
		_is_bottom_left = true;
	}
	else if (downKey && rightKey)
	{
		_is_bottom_right = true;
	}
	else
	{
		if (upKey)
		{
			_is_up = true;
		}
		if (leftKey)
		{
			_is_left = true;
		}
		if (downKey)
		{
			_is_down = true;
		}
		if (rightKey)
		{
			_is_right = true;
		}
	}
}

auto LadderState::Tick(const double timeDelta) -> void
{
	if (_is_jump)
	{
		return;
	}

	auto transform = _player->GetTransform();
	if (_is_left)
	{
		transform->WalkLeft(timeDelta);
	}
	else if (_is_right)
	{
		transform->WalkRight(timeDelta);
	}
	else if (_is_up)
	{
		transform->WalkUp(timeDelta);
	}
	else if (_is_down)
	{
		transform->WalkDown(timeDelta);
	}

	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
	if (LadderMoveCheck())
	{
		if (_is_left)
		{
			transform->WalkLeft(-timeDelta);
		}
		else if (_is_right)
		{
			transform->WalkRight(-timeDelta);
		}
		else if (_is_up)
		{
			transform->WalkUp(-timeDelta);
		}
		else if (_is_down)
		{
			transform->WalkDown(-timeDelta);
		}
	}

	_player->GetCharacterColliderAabb()->UpdateCollider();
	range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}

}

auto LadderState::LateTick(const double timeDelta) -> void
{
	_player->PlayAnimation(timeDelta);



	const auto gender = _player->GetInfo().sex;
	const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);

	if (_is_left)
	{
		if (_player->GetCurrentAnimationType() != kAnimationType::kLadderLeftMove)
		{
			_player->ChangeAnimation(kAnimationType::kLadderLeftMove);
		}
	}
	else if (_is_right)
	{
		if (_player->GetCurrentAnimationType() != kAnimationType::kLadderRightMove)
		{
			_player->ChangeAnimation(kAnimationType::kLadderRightMove);
		}
	}
	else if (_is_up)
	{
		// 15 17
		if (_player->GetCurrentAnimationType() == kAnimationType::kLadderLeftUp)
		{
			if (_player->GetAnimationTimeAcc() >= kfm->seqs[15]->key[L"end"])
			{
				_player->ChangeAnimation(kAnimationType::kLadderRightUp);
			}
		}
		else if (_player->GetCurrentAnimationType() == kAnimationType::kLadderRightUp)
		{
			if (_player->GetAnimationTimeAcc() >= kfm->seqs[17]->key[L"end"])
			{
				_player->ChangeAnimation(kAnimationType::kLadderLeftUp);
			}
		}
		else
		{
			_player->ChangeAnimation(kAnimationType::kLadderLeftUp);
		}
	}
	else if (_is_down)
	{
		//14 16
		if (_player->GetCurrentAnimationType() == kAnimationType::kLadderLeftDown)
		{
			if (_player->GetAnimationTimeAcc() >= kfm->seqs[14]->key[L"end"])
			{
				_player->ChangeAnimation(kAnimationType::kLadderRightDown);
			}
		}
		else if (_player->GetCurrentAnimationType() == kAnimationType::kLadderRightDown)
		{
			if (_player->GetAnimationTimeAcc() >= kfm->seqs[16]->key[L"end"])
			{
				_player->ChangeAnimation(kAnimationType::kLadderLeftDown);
			}
		}
		else
		{
			_player->ChangeAnimation(kAnimationType::kLadderLeftDown);
		}
	}
	else
	{
		if (_player->GetCurrentAnimationType() != kAnimationType::kLadderIdle)
		{
			_player->ChangeAnimation(kAnimationType::kLadderIdle);
		}
	}
	if (_is_jump)
	{
		_player->ChangeCharacterState(JumpState::GetInstance());
	}

}

auto LadderState::Render() -> void
{
}
