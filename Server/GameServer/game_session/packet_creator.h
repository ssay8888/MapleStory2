#pragma once
class Monster;
class GameSession;

class PacketCreator
{
public:
	static void AcceptEnterGame(std::shared_ptr<GameSession> session);
	static void SpawnPlayer(std::shared_ptr<GameSession> session, int64 player_id);
	static void AddUser(std::shared_ptr<GameSession> new_session, std::shared_ptr<GameSession> target_session);
	static void MovePlayer(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> target_session);
	static void RemovePlayer(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> target_session);
	static void RespawnMonster(std::shared_ptr<GameSession> session, std::shared_ptr<Monster> monster);
	static void RemoveMonster(std::shared_ptr<GameSession> session, std::shared_ptr<Monster> monster);
	static void ChangeScale(std::shared_ptr<GameSession> session, std::shared_ptr<GameSession> targetSession, bool isPlayer);
	static void ConnectedUserCount(std::shared_ptr<GameSession> session, int64 count);
};

