#pragma once

class Player;
class GameRoomManager
{
private:
	GameRoomManager();
	~GameRoomManager() = default;
public:
	static auto GetInstance() -> GameRoomManager&
	{
		static GameRoomManager instance;
		return instance;
	}

	void InitRoom();

	auto EnterUsesr(int16 number, PacketSessionRef session) -> void;
	auto LeaveUsesr(int16 number, PacketSessionRef session) -> void;


private:
	Vector<std::shared_ptr<class GameRoom>> _rooms;

};

