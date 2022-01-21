#include "c_pch.h"
#include "game_server_packet_handler.h"

#include "src/game_job_queue/game_logic_queue.h"

PacketHandlerFunc GameServerPacketHandler::_packet_handler[UINT16_MAX];
auto GameServerPacketHandler::HandleGameInvalid(PacketSessionRef& session, BYTE* buffer, int32_t len) -> bool
{
	return false;
}

auto GameServerPacketHandler::HandleGameServerLogin(PacketSessionRef& session, Protocol::GameServerLogin& pkt) -> bool
{
	return true;
}

auto GameServerPacketHandler::HandleGameServerLoadCharacter(PacketSessionRef& session,
	Protocol::GameServerLoadCharacter& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::CharacterLoad, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerRespawnPlayer(PacketSessionRef& session,
	Protocol::GameServerRespawnPlayer& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::AddUser, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerMovePlayer(PacketSessionRef& session,
	Protocol::GameServerMovePlayer& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::MovePlayer, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerRespawnMonster(PacketSessionRef& session,
	Protocol::GameServerRespawnMonster& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::RespawnMonster, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerMoveMonster(PacketSessionRef& session,
	Protocol::GameServerMoveMonster& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::MoveMonster, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerTakeDamage(PacketSessionRef& session,
	Protocol::GameServerTakeDamage& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::TakeDamage, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerUpdateStat(PacketSessionRef& session,
	Protocol::GameServerUpdateStat& pkt) -> bool
{

	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::UpdateStat, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerAttackMonster(PacketSessionRef& session,
	Protocol::GameServerAttackMonster& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::AttackMonster, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerMonsterStatUpdate(PacketSessionRef& session,
	Protocol::GameServerMonsterStatUpdate& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::UpdateMonsterStat, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerKillMonster(PacketSessionRef& session,
                                                          Protocol::GameServerKillMonster& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::KillMonster, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerDressChange(PacketSessionRef& session,
	Protocol::GameServerDressChange& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::DressChange, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerStatUp(PacketSessionRef& session, Protocol::GameServerStatUp& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::StatupChange, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerItemQuantityUpdate(PacketSessionRef& session,
	Protocol::GameServerItemQuantityUpdate& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::ItemQuantityUpdate, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerResurrection(PacketSessionRef& session,
	Protocol::GameServerResurrection& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::ResurrectionPlayer, session, pkt);
	return true;
}

auto GameServerPacketHandler::HandleGameServerChat(PacketSessionRef& session, Protocol::GameServerChat& pkt) -> bool
{
	GameLogicQueue::GetInstance()->DoAsync(&GameLogicQueue::GameChat, session, pkt);
	return true;
}
