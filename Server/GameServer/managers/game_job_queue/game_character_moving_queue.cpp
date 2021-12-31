#include "game_server_pch.h"
#include "game_character_moving_queue.h"

#include "game/entitiy/character/game_character.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "src/utility/components/transform/transform.h"

auto GameCharacterMovingQueue::MovePlayer(PacketSessionRef session, Protocol::GameClientMovePlayer pkt) -> void
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	const auto mapInstance = MapManager::GetInstance().FindMapInstance(gameSession->GetPlayer()->GetMapId());

	const auto transForm = gameSession->GetPlayer()->GetTransForm();
	transForm->SetUpRotation(_float3(0.f, 1.f, 0.f), pkt.radian());
	const auto pos = _float3(pkt.position().x(), pkt.position().y(), pkt.position().z());
	transForm->SetState(Transform::kState::kStatePosition, pos);

	Protocol::GameServerMovePlayer sendPkt;
	sendPkt.set_character_id(gameSession->GetPlayer()->GetCharacterId());
	sendPkt.set_state(pkt.state());
	sendPkt.set_radian(pkt.radian());
	const auto position = sendPkt.mutable_position();
	position->set_x(pos.x);
	position->set_y(pos.y);
	position->set_z(pos.z);
	mapInstance->BroadCastMessage(sendPkt, gameSession);
}
