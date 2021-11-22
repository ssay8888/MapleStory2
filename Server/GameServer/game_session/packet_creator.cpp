#include "pch.h"
#include "packet_creator.h"

#include <d3dx9math.h>

#include "client_packet_handler.h"
#include <src/utils/buffer_writer.h>
#include <src/network/session.h>

#include "game_session.h"
#include "game_session_manaeger.h"
#include "game_contents/game_objects/monster/monster.h"
#include "player/player.h"

void PacketCreator::AcceptEnterGame(std::shared_ptr<GameSession> session)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();
	header->size = bw.WriteSize();
	header->id = PKT_S_LOGIN;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}

void PacketCreator::SpawnPlayer(std::shared_ptr<GameSession> session, int64 player_id)
{

	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << player_id;
	bw << session->GetPlayer()->GetPos();

	header->size = bw.WriteSize();
	header->id = PKT_S_SPAWNPLAYER;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}

void PacketCreator::AddUser(std::shared_ptr<GameSession> new_session, std::shared_ptr<GameSession> target_session)
{
	if (target_session->GetPlayer() == nullptr)
	{
		return;
	}
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << target_session->GetSessionId();
	bw << target_session->GetPlayer()->GetPos();

	header->size = bw.WriteSize();
	header->id = PKT_S_ADDPLAYER;

	sendBuffer->Close(bw.WriteSize());

	new_session->Send(sendBuffer);
}

void PacketCreator::MovePlayer(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> target_session)
{
	if (session->GetPlayer() == nullptr)
	{
		return;
	}
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << session->GetSessionId();
	bw << session->GetPlayer()->GetPos();

	header->size = bw.WriteSize();
	header->id = PKT_S_MOVEPLAYER;

	sendBuffer->Close(bw.WriteSize());

	target_session->Send(sendBuffer);
}

void PacketCreator::RemovePlayer(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> target_session)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << session->GetSessionId();

	header->size = bw.WriteSize();
	header->id = PKT_S_REMOVEPLAYER;

	sendBuffer->Close(bw.WriteSize());

	target_session->Send(sendBuffer);
}

void PacketCreator::RespawnMonster(std::shared_ptr<GameSession> session, std::shared_ptr<Monster> monster)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << monster->GetObjectId();
	bw << monster->GetExp();
	bw << monster->GetWorldPos();

	header->size = bw.WriteSize();
	header->id = PKT_S_RESPAWNMONSTER;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}

void PacketCreator::RemoveMonster(std::shared_ptr<GameSession> session, std::shared_ptr<Monster> monster)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << monster->GetObjectId();

	header->size = bw.WriteSize();
	header->id = PKT_S_REMOVEMONSTER;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}

void PacketCreator::ChangeScale(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> targetSession, bool isPlayer)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << targetSession->GetSessionId();
	bw << isPlayer;
	bw << targetSession->GetPlayer()->GetScale();

	header->size = bw.WriteSize();
	header->id = PKT_S_CHANGESCALE;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}

void PacketCreator::ConnectedUserCount(std::shared_ptr<GameSession> session, int64 count)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), 4096);
	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << count;

	header->size = bw.WriteSize();
	header->id = PKT_S_CONNECTEDUSER;

	sendBuffer->Close(bw.WriteSize());

	session->Send(sendBuffer);
}
