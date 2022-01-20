#include "c_pch.h"
#include "down_idle_state.h"

#include "protocol/game_protocol.pb.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/idle_state/idle_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto DownIdleState::Enter() -> void
{
	_is_move = false;
	_dead_time = GetTickCount64();
	_player->ChangeAnimation(kAnimationType::kDownIdle);

	const auto transform = _player->GetTransform();
	Protocol::GameClientMovePlayer sendPkt;
	sendPkt.set_state(static_cast<int32_t>(kAnimationType::kDownIdle));
	sendPkt.set_radian(_player->GetRadian());
	const auto playerPos = transform->GetState(Transform::kState::kStatePosition);
	const auto position = sendPkt.mutable_position();
	position->set_x(playerPos.x);
	position->set_y(playerPos.y);
	position->set_z(playerPos.z);
	SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
}

auto DownIdleState::HandleInput() -> void
{
	if (!g_isWindowsActive)
	{
		return;
	}
	if (InputDevice::GetInstance().GetKeyPressing(DIK_UP))
	{
		_is_move = true;
	}
}

auto DownIdleState::Tick(const double timeDelta) -> void
{
	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
	if (_dead_time + 5000 < GetTickCount64())
	{
		Protocol::GameClientResurrection sendPkt;
		SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
	}
}

auto DownIdleState::LateTick(const double timeDelta) -> void
{
	ReloadMapObject();
	auto check = GravityPlayer(timeDelta);
	if (check)
	{
		GravityPlayerSendMessage(kAnimationType::kIdle);
	}
	_player->PlayAnimation(timeDelta);
	if (_is_move)
	{
		//_player->ChangeCharacterState(IdleState::GetInstance());
	}
}

auto DownIdleState::Render() -> void
{
}
