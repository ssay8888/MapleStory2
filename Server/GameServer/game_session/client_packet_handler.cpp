#include "pch.h"
#include "client_packet_handler.h"

#include <d3dx9math.h>

#include "game_session_manaeger.h"
#include "game_session/packet_creator.h"
#include "game_contents/game_room/game_room_manager.h"
#include "src/utils/buffer_reader.cpp"
#include "game_session.h"
#include "game_contents/game_room/game_room.h"
#include "player/player.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef session, BYTE* buffer, int32 len)
{
	std::cout << "Invalid" << std::endl;
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef session, Protocol::C_LOGIN& pkt)
{
	std::cout << "Login" << std::endl;
	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	PacketCreator::AcceptEnterGame(gameSession);
	return true;
}

bool Handle_C_INGAME(PacketSessionRef session, BYTE* buffer, int32 len)
{
	std::cout << "HelloInGame" << std::endl;
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;
	int number;
	br >> number;

	auto& instance = GameRoomManager::GetInstance();
	instance.EnterUsesr(number, session);
	std::cout << number << "·ë ÀÔÀå" << std::endl;
	return true;
}

bool Handle_C_MOVEPLAYER(PacketSessionRef session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	D3DXMATRIX world;
	br >> world;
	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);

	gameSession->GetPlayer()->SetPos(world);
	auto room = gameSession->GetPlayer()->GetRoom();
	room->DoAsync(&GameRoom::BroadCastMovePlayer, gameSession);
	return true;
}

bool Handle_C_MONSTERCOLLISTION(PacketSessionRef session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	int64 object_id;
	br >> object_id;

	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	auto room = gameSession->GetPlayer()->GetRoom();
	room->DoAsync(&GameRoom::DeadMonster, session, object_id);
	return true;
}

bool Handle_C_USERCOLLISTION(PacketSessionRef session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	int64 object_id;
	br >> object_id;

	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	auto room = gameSession->GetPlayer()->GetRoom();
	room->DoAsync(&GameRoom::UserCollistion, session, object_id);
	return true;
}
