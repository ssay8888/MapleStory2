#include "game_server_pch.h"
#include "game_client_packet_handler.h"
#include "game_tick/game_tick.h"
#include "managers/game_job_queue/game_character_load_queue.h"
#include "managers/game_job_queue/game_character_moving_queue.h"

PacketHandlerFunc GameClientPacketHandler::_packet_handler[UINT16_MAX]{};

auto GameClientPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameClientPacketHandler::HandleGameClientLogin(PacketSessionRef& session, Protocol::GameClientLogin& pkt) -> bool
{
	GameCharacterLoadQueue::GetInstance()->DoAsync(&GameCharacterLoadQueue::GameClientLoginResponse, session, pkt);
	return true;
}

auto GameClientPacketHandler::HandleGameClientLoading(PacketSessionRef& session,
	Protocol::GameClientLoading& pkt) -> bool
{
	GameCharacterLoadQueue::GetInstance()->DoAsync(&GameCharacterLoadQueue::GameClientLoadingResponse, session, pkt);
	return true;
}

auto GameClientPacketHandler::HandleGameClientMovePlayer(PacketSessionRef& session,
                                                         Protocol::GameClientMovePlayer& pkt) -> bool
{
	GameCharacterMovingQueue::GetInstance()->DoAsync(&GameCharacterMovingQueue::MovePlayer, session, pkt);
	return true;
}

auto GameClientPacketHandler::HandleGameClientTakeDamage(PacketSessionRef& session,
	Protocol::GameClientTakeDamage& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::TakeDamage, pkt.character_id(), pkt.monster_obj_id(), gameSession);
	return true;
}

auto GameClientPacketHandler::HandleGameClientAttackMonster(PacketSessionRef& session,
	Protocol::GameClientAttackMonster& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::AttackMonster, pkt, gameSession);
	return true;
}

auto GameClientPacketHandler::HandleGameClientInventoryItemMove(PacketSessionRef& session,
	Protocol::GameClientInventoryItemMove& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::InventoryItemMove, pkt, gameSession);
	return true;
}

auto GameClientPacketHandler::HandleGameClientStatUp(PacketSessionRef& session, Protocol::GameClientStatUp& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::PlayerStatUp, pkt, gameSession);
	return true;
}

auto GameClientPacketHandler::HandleGameClientKeySet(PacketSessionRef& session, Protocol::GameClientKeySet& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::PlayerKeySetChange, pkt, gameSession);
	return true;
}

auto GameClientPacketHandler::HandleGameClientItemApply(PacketSessionRef& session,
	Protocol::GameClientItemApply& pkt) -> bool
{
	const auto gameSession = std::static_pointer_cast<GameSession>(session);
	GameTick::GetInstance()->DoAsync(&GameTick::PlayerItemApply, pkt, gameSession);
	return true;
}
