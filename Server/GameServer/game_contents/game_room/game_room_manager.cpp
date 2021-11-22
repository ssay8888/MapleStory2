#include "pch.h"
#include "game_room_manager.h"
#include "game_room.h"

GameRoomManager::GameRoomManager()
{
	InitRoom();
}

inline void GameRoomManager::InitRoom()
{
	_rooms.reserve(10);
	for (int i = 0; i < 10; i++)
	{
		_rooms.push_back(std::make_shared<GameRoom>());
	}
}

auto GameRoomManager::EnterUsesr(const int16 number, PacketSessionRef session) -> void
{
	if (_rooms.size() <= number)
	{
		return;
	}
	auto& room = _rooms[number];
	room->DoAsync(&GameRoom::Enter, session);
}

auto GameRoomManager::LeaveUsesr(int16 number, PacketSessionRef session) -> void
{
	if (_rooms.size() <= number)
	{
		return;
	}
	auto& room = _rooms[number];
	room->DoAsync(&GameRoom::Leave, session);
}
