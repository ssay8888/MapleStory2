#include "c_pch.h"
#include "down_state.h"

#include "data_reader/data_reader_manager.h"
#include "protocol/game_protocol.pb.h"
#include "src/game_object/player/player.h"
#include "src/game_object/player/states/down_idle_state/down_idle_state.h"
#include "src/network/game_server_packet_handler.h"
#include "src/network/send_manager.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto DownState::Enter() -> void
{
	_player->ChangeAnimation(kAnimationType::kDown);
	const auto transform = _player->GetTransform();
	Protocol::GameClientMovePlayer sendPkt;
	sendPkt.set_state(static_cast<int32_t>(kAnimationType::kDown));
	sendPkt.set_radian(_player->GetRadian());
	const auto playerPos = transform->GetState(Transform::kState::kStatePosition);
	const auto position = sendPkt.mutable_position();
	position->set_x(playerPos.x);
	position->set_y(playerPos.y);
	position->set_z(playerPos.z);
	SendManager::GetInstance().Push(GameServerPacketHandler::MakeSendBuffer(sendPkt));
}

auto DownState::HandleInput() -> void
{
	if (!g_isWindowsActive)
	{
		return;
	}
}

auto DownState::Tick(const double timeDelta) -> void
{
	_player->GetCharacterColliderAabb()->UpdateCollider();
	auto range = _player->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
}

auto DownState::LateTick(const double timeDelta) -> void
{
	ReloadMapObject();
	auto check = GravityPlayer(timeDelta);
	if (check)
	{
		GravityPlayerSendMessage(kAnimationType::kIdle);
	}
	_player->PlayAnimation(timeDelta);
	const auto gender = _player->GetInfo().sex;
	const auto kfm = DataReaderManager::GetInstance().FindPlayerAniKey(gender);
	const auto seq = kfm->seqs[gender ? 166 : 167];
	const auto endTime = seq->key[L"end"];
	if (_player->GetAnimationTimeAcc() >= endTime - 0.25)
	{
		_player->ChangeCharacterState(DownIdleState::GetInstance());
	}
}

auto DownState::Render() -> void
{
}
